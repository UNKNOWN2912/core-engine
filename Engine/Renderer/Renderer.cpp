#include "Renderer.hpp"
#include "Assets/ShaderManager.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Helper.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <cstring>

void Renderer::Initialize(const RendererSpecification &specification)
{
    mSpecification = specification;
    GraphicsContext::Initialize(mSpecification.deviceType);

    TextureManager::Initialize();

    mShadowMapDescriptor.AddBindlessDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment, 1024);
    mShadowMapDescriptor.CreateDescriptor();

    CreateSceneRenderPassMultisampled();
    CreateSceneAttachmentsMultisampled();
    CreateSceneFrameBufferMultisampled();
    mCommandBuffer.CreateCommandBuffer();

    mSampler.CreateSampler();

    mDirectionalShadowSampler.SetFilter(Filter::Linear, Filter::Linear);
    mDirectionalShadowSampler.SetAddressMode(AddressMode::Border, AddressMode::Border, AddressMode::Border);
    mDirectionalShadowSampler.CreateSampler();

    mPresentInputDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment);
    mPresentInputDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment);
    mPresentInputDescriptor.AddDescriptor(DescriptorType::StorageImage, ShaderStage::Fragment);
    mPresentInputDescriptor.CreateDescriptor();
    mPresentInputDescriptor.UpdateImage(mSceneResolveAttachment, ImageLayout::ShaderRead, mSampler, 0);
    mPresentInputDescriptor.UpdateImage(mSceneResolveDepthAttachment, ImageLayout::ShaderRead, mSampler, 1);

    CreatePresentRenderPass();
    CreatePresentPipeline();
    mImageAcquiredSemaphore.CreateSemaphore();
    mPresentCommandBuffer.CreateCommandBuffer();
    mSwapchainRenderFinished.CreateSemaphore();

    mLight.reserve(1000);

    mUniformBuffer = UniformBuffer(sizeof(UniformData));
    mLightStorageBuffer.CreateStorageBuffer(nullptr, sizeof(Light) * maxLightCount);
}

void Renderer::Terminate()
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    TextureManager::Terminate();

    DestroyImage(mSceneColorAttachment);
    DestroyImage(mSceneResolveAttachment);
    mSceneFrameBuffer.DestroyFrameBuffer();
    mScenePipeline.DestroyPipeline();
    mSceneRenderPass.DestroyRenderPass();
    mCommandBuffer.DestroyCommandBuffer();
    mPresentCommandBuffer.DestroyCommandBuffer();
    mPresentInputDescriptor.DestroyDescriptor();
    mPresentPipeline.DestroyPipeline();
    mPresentRenderPass.DestroyRenderPass();

    mUniformBuffer.DestroyUniformBuffer();

    for (auto &[material, renderObject] : mMaterialObjectMap)
    {
        renderObject.bufferDescriptor.DestroyDescriptor();
        renderObject.pipeline.DestroyPipeline();
        renderObject.textureDescriptor.DestroyDescriptor();
        renderObject.sampler.DestroySampler();
        renderObject.shadowMapDescriptor.DestroyDescriptor();
    }

    mSampler.DestroySampler();
}

void Renderer::BeginFrame(const Camera &camera)
{
    mRenderCommands.clear();
    mFrameInfo.recording = true;

    mUniformData.view = camera.GetView();
    mUniformData.projection = camera.GetProjection();
    mUniformData.cameraPosition = camera.GetPosition();
    mUniformData.cameraFront = camera.GetFront();
    mUniformData.lightCount = (int)mLight.size();
    mUniformBuffer.SetData(&mUniformData);
}

void Renderer::EndFrame()
{
    assert(mFrameInfo.recording);
    mFrameInfo = FrameInfo();

    mCommandBuffer.BeginRecording();

    mSceneRenderPass.CmdBeginRenderPass(mCommandBuffer, mSceneFrameBuffer, mResolution, {{0, 0, 0, 1}, {0, 0, 0, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}});

    for (const RenderCommand &renderCommand : mRenderCommands)
    {
        CmdDrawRenderCommand(renderCommand);
    }

    mSceneRenderPass.CmdEndRenderPass(mCommandBuffer);

    mCommandBuffer.EndRecording();

    mCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics);
}
const glm::uvec2 &Renderer::GetResolution()
{
    return mResolution;
}
SampleCount Renderer::GetSampleCount()
{
    return Renderer::mSampleCount;
}

void Renderer::SetSampleCount(const SampleCount &sampleCount)
{
    Renderer::mSampleCount = sampleCount;
};

Surface Renderer::CreateSurface(const Window &window)
{
    Surface surface;
    surface.handle = window.CreateWindowSurface();
    surface.swapchain.CreateSwapchain(surface.handle, ImageFormat::BGRA8, ColorSpace::SRGBNonLinear, PresentMode::Fifo);

    for (const ImageDeprecated &image : surface.swapchain.GetImages())
    {
        FrameBuffer frameBuffer;
        frameBuffer.CreateFrameBuffer({image}, mPresentRenderPass);
        surface.frameBuffers.emplace_back(frameBuffer);
    }

    return surface;
}

void Renderer::ResizeSurface(Surface &surface)
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());
    for (auto &framebuffer : surface.frameBuffers)
    {
        framebuffer.DestroyFrameBuffer();
    }
    surface.frameBuffers.clear();

    surface.swapchain.DestroySwapchain();

    surface.swapchain.CreateSwapchain(surface.handle, ImageFormat::BGRA8, ColorSpace::SRGBNonLinear, PresentMode::Fifo);

    for (const ImageDeprecated &image : surface.swapchain.GetImages())
    {
        FrameBuffer frameBuffer;
        frameBuffer.CreateFrameBuffer({image}, mPresentRenderPass);
        surface.frameBuffers.emplace_back(frameBuffer);
    }
}

void Renderer::Present(Surface &surface)
{
    uint32_t imageIndex = surface.swapchain.GetNextImageIndex(mImageAcquiredSemaphore, {});
    if (imageIndex == UINT32_MAX)
    {
        return;
    }

    mPresentCommandBuffer.BeginRecording();
    mPresentRenderPass.CmdBeginRenderPass(mPresentCommandBuffer, surface.frameBuffers[imageIndex], surface.swapchain.GetSize(), {{1, 1, 1, 1}});

    VkViewport viewport =
        {
            .width = (float)surface.swapchain.GetSize().x,
            .height = (float)surface.swapchain.GetSize().y,
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };

    VkRect2D scissor =
        {
            .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
        };

    vkCmdSetViewport(mPresentCommandBuffer.GetHandle(), 0, 1, &viewport);
    vkCmdSetScissor(mPresentCommandBuffer.GetHandle(), 0, 1, &scissor);

    VkDescriptorSet descriptorSets[] = {mPresentInputDescriptor.GetDescriptorSet()};
    vkCmdBindDescriptorSets(mPresentCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPresentPipeline.GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

    mPresentPipeline.CmdBindPipeline(mPresentCommandBuffer);

    vkCmdDraw(mPresentCommandBuffer.GetHandle(), 6, 1, 0, 0);

    mPresentRenderPass.CmdEndRenderPass(mPresentCommandBuffer);
    mPresentCommandBuffer.EndRecording();

    mPresentCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics, mImageAcquiredSemaphore, mSwapchainRenderFinished, PipelineStage::ColorAttachmentOutput);

    VkSwapchainKHR swapchain[] = {surface.swapchain.GetHandle()};
    VkSemaphore waitSemaphores[] = {mSwapchainRenderFinished.GetHandle()};

    VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapchain,
            .pImageIndices = &imageIndex,
        };

    vkQueuePresentKHR(GraphicsContext::GetQueues().graphics, &presentInfo);

    vkDeviceWaitIdle(GraphicsContext::GetDevice());
}

void Renderer::RegisterMaterial(const Material &material)
{
    CreateRendererMaterialObject(material, mMaterialObjectMap[&material]);
}
const std::vector<RenderCommand> &Renderer::GetRenderCommands()
{
    return mRenderCommands;
}

void Renderer::Submit(RenderCommand renderCommand)
{
    mRenderCommands.push_back(renderCommand);
}

void Renderer::Submit(const Mesh &mesh, const Material &material, const Transform &transform)
{

    RenderCommand renderCommand;
    renderCommand.vertexBuffer = &mesh.GetVertexBuffer();
    renderCommand.indexBuffer = &mesh.GetIndexBuffer();
    renderCommand.descriptorCount = 4;
    renderCommand.descriptors[0] = &mMaterialObjectMap[&material].textureDescriptor;
    renderCommand.descriptors[1] = &mMaterialObjectMap[&material].bufferDescriptor;
    renderCommand.descriptors[2] = &mShadowMapDescriptor;
    renderCommand.descriptors[3] = &TextureManager::GetDescriptor();
    renderCommand.pipeline = &mMaterialObjectMap[&material].pipeline;
    renderCommand.indexCount = mesh.mIndexSize / sizeof(uint32_t);

    PushConstantData data;
    data.model = transform.GetMatrix();
    data.albedoIndex = (uint32_t)material.albedo;

    memcpy(renderCommand.pushContantData, &data, sizeof(data));
    renderCommand.pushContantSize = sizeof(data);

    mRenderCommands.push_back(renderCommand);
}

void Renderer::Submit(MaterialID material, MeshID mesh, const Transform &transform)
{
    Submit(*MeshManager::GetMesh(mesh), *MaterialManager::GetMaterial(material), transform);
}

void Renderer::SetBasicShader(std::string_view vertexShader, std::string_view fragmentShader)
{
    mBasicVertexShader = ShaderManager::LoadVertexShader(vertexShader);
    mBasicFragmentShader = ShaderManager::LoadFragmentShader(fragmentShader);
}

void Renderer::GetBasicShader(VertexShaderID &vertexShader, FragmentShaderID &fragmentShader)
{
    vertexShader = mBasicVertexShader;
    fragmentShader = mBasicFragmentShader;
}

void Renderer::AddLight(const Light &light)
{
    uint32_t index = mShadowMaps.size();
    mShadowMaps.push_back(light.GetShadowMap());

    if (light.GetType() == LightType::DirectionalLight)
    {
        mUniformData.directionalMatrix1 = light.GetDirectionalProjection(0);
        mUniformData.directionalMatrix2 = light.GetDirectionalProjection(1);
        mUniformData.directionalMatrix3 = light.GetDirectionalProjection(2);
        mUniformData.directionalMatrix4 = light.GetDirectionalProjection(3);
    }

    LightUniformData data =
        {
            .position = light.GetPosition(),
            .intensity = light.GetIntensity(),
            .color = light.GetColor(),
            .innerAngle = light.GetInnerCosinAngle(),
            .direction = light.GetDirection(),
            .outerAngle = light.GetOuterCosinAngle(),
            .radius = 1,
            .type = (int)light.GetType(),
            .shadowMapIndex = (int)index,
            .castShadow = true,
        };

    mLight.push_back(data);
}

void Renderer::BeginLightPlacement()
{
    mLight.clear();
    mShadowMaps.clear();
}
void Renderer::EndLightPlacement()
{
    mLightStorageBuffer.SetData(mLight.data(), sizeof(LightUniformData) * mLight.size());

    for (int i = 0; i < mShadowMaps.size(); i++)
    {
        mShadowMapDescriptor.UpdateImageIndex(mShadowMaps[i], ImageLayout::ShaderRead, mDirectionalShadowSampler, 0, i);
    }
}
void Renderer::SetProjectionMatrix(const glm::mat4 &matrix)
{
    mUniformData.projection = matrix;
}
void Renderer::SetViewMatrix(const glm::mat4 &matrix)
{
    mUniformData.projection = matrix;
}

void Renderer::CreateSceneRenderPassMultisampled()
{
    uint32_t colorResolve = mSceneRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    uint32_t colorAttachment = mSceneRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::None, ImageLayout::ColorAttachment, LoadOperation::Clear, StoreOperation::DontCare, LoadOperation::DontCare, StoreOperation::DontCare, mSampleCount);
    uint32_t depthResolve = mSceneRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    uint32_t depthAttachment = mSceneRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::DepthStencil, LoadOperation::Clear, StoreOperation::DontCare, LoadOperation::DontCare, StoreOperation::DontCare, mSampleCount);

    Subpass subpass;
    subpass.AddColorAttachment(colorAttachment);
    subpass.AddResolveAttachment(colorResolve);
    subpass.SetDepthAttachment(depthAttachment);
    subpass.SetDepthResolveAttachment(depthResolve);

    mSceneRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);

    mSceneRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);

    mSceneRenderPass.CreateRenderPass();
}

void Renderer::CreateSceneFrameBufferMultisampled()
{
    mSceneFrameBuffer.CreateFrameBuffer({mSceneResolveAttachment, mSceneColorAttachment, mSceneResolveDepthAttachment, mSceneDepthAttachment}, mSceneRenderPass);
}

void Renderer::CreateSceneAttachmentsMultisampled()
{
    mSceneColorAttachment = CreateImage(mResolution, ImageFormat::BGRA8, ImageUsage::ColorAttachment, ImageAspect::Color, MemoryProperty::DeviceLocal, mSampleCount);
    mSceneResolveAttachment = CreateImage(mResolution, ImageFormat::BGRA8, ImageUsage::ColorAttachment | ImageUsage::Sampler | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal, SampleCount::One);
    mSceneDepthAttachment = CreateImage(mResolution, ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal, mSampleCount);
    mSceneResolveDepthAttachment = CreateImage(mResolution, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
}

void Renderer::CreateScenePipelineMultisampled()
{
    VertexShaderID vertexID = ShaderManager::LoadVertexShader("Shaders/test.vert.spv");
    FragmentShaderID fragmentID = ShaderManager::LoadFragmentShader("Shaders/test.frag.spv");

    mScenePipeline.SetVertexShader(ShaderManager::GetVertexShader(vertexID));
    mScenePipeline.SetFragmentShader(ShaderManager::GetFragmentShader(fragmentID));

    mScenePipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    mScenePipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mScenePipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    mScenePipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));

    mScenePipeline.SetMultisampleCount(mSampleCount);
    mScenePipeline.AddColorBlendAttachment(false);
    mScenePipeline.SetCullMode(CullMode::None);
    mScenePipeline.CreatePipeline(mSceneRenderPass, 0);
}

void Renderer::CreatePresentPipeline()
{
    VertexShaderID vertexId = ShaderManager::LoadVertexShader("Shaders/fullscreen.vert.spv");
    FragmentShaderID fragmentId = ShaderManager::LoadFragmentShader("Shaders/fullscreen.frag.spv");

    mPresentPipeline.AddDescriptors(mPresentInputDescriptor);

    mPresentPipeline.AddColorBlendAttachment(false);

    mPresentPipeline.SetVertexShader(ShaderManager::GetVertexShader(vertexId));
    mPresentPipeline.SetFragmentShader(ShaderManager::GetFragmentShader(fragmentId));

    mPresentPipeline.SetCullMode(CullMode::None);

    mPresentPipeline.CreatePipeline(mPresentRenderPass, 0);
}

void Renderer::CreatePresentRenderPass()
{
    mPresentRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::None, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);

    Subpass subpass;
    subpass.AddColorAttachment(0);
    mPresentRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);
    mPresentRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mPresentRenderPass.CreateRenderPass();
}

void Renderer::CreateRendererMaterialObject(const Material &material, RendererMaterialObject &object)
{
    if (object.sampler.GetHandle() != VK_NULL_HANDLE)
    {
        object.sampler.DestroySampler();
        object.textureDescriptor.DestroyDescriptor();
        object.bufferDescriptor.DestroyDescriptor();
        object.pipeline.DestroyPipeline();
    }

    object.sampler.SetFilter(material.minFilter, material.magFilter);
    object.sampler.SetAddressMode(material.addressMode, material.addressMode, material.addressMode);
    object.sampler.CreateSampler();

    object.textureDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment);
    object.textureDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment);
    object.textureDescriptor.CreateDescriptor();
    if (material.albedo != (TextureID)UINT64_MAX)
    {
        object.textureDescriptor.UpdateImage(TextureManager::GetTexture(material.albedo)->GetImage(), ImageLayout::ShaderRead, object.sampler, 0);
    }

    object.bufferDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    object.bufferDescriptor.AddDescriptor(DescriptorType::StorageBuffer, ShaderStage::Fragment);
    object.bufferDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Fragment);
    object.bufferDescriptor.CreateDescriptor();
    object.bufferDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);
    object.bufferDescriptor.UpdateBuffer(mLightStorageBuffer.GetBuffer(), 1);

    object.pipeline.SetVertexShader(ShaderManager::GetVertexShader(material.vertexShader));
    object.pipeline.SetFragmentShader(ShaderManager::GetFragmentShader(material.fragmentShader));
    if (material.geometryShader != ShaderManager::GetInvalidGeometryShaderID())
    {
        object.pipeline.SetGeometryShader(ShaderManager::GetGeometryShader(material.geometryShader));
    }

    object.pipeline.AddDescriptors(object.textureDescriptor);
    object.pipeline.AddDescriptors(object.bufferDescriptor);
    object.pipeline.AddDescriptors(mShadowMapDescriptor);
    object.pipeline.AddDescriptors(TextureManager::GetDescriptor());

    object.pipeline.SetCullMode(material.cullMode);
    object.pipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    object.pipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    object.pipeline.AddAttribute(0, 1, ImageFormat::RGB32, offsetof(Vertex, uv));
    object.pipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));
    object.pipeline.EnableDepthWrite(material.enableDepthWrite);
    object.pipeline.EnableDepthTesting(material.enableDepthTest);
    object.pipeline.SetMultisampleCount(mSampleCount);
    object.pipeline.SetPushConstant(ShaderStage::All, sizeof(PushConstantData));
    object.pipeline.AddColorBlendAttachment(true);
    object.pipeline.CreatePipeline(mSceneRenderPass, 0);
}

void Renderer::CmdDrawRenderCommand(const RenderCommand &renderCommand)
{
    VkBuffer vertexBuffer[] = {renderCommand.vertexBuffer->handle};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(mCommandBuffer.GetHandle(), 0, 1, vertexBuffer, offsets);
    vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);

    VkViewport viewport =
        {
            .width = (float)mResolution.x,
            .height = (float)mResolution.y,
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };

    VkRect2D scissor =
        {
            .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
        };

    vkCmdSetViewport(mCommandBuffer.GetHandle(), 0, 1, &viewport);
    vkCmdSetScissor(mCommandBuffer.GetHandle(), 0, 1, &scissor);
    vkCmdPushConstants(mCommandBuffer.GetHandle(), renderCommand.pipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, renderCommand.pushContantSize, renderCommand.pushContantData);

    VkDescriptorSet descriptorSets[32];
    for (int i = 0; i < renderCommand.descriptorCount; i++)
    {
        descriptorSets[i] = renderCommand.descriptors[i]->GetDescriptorSet();
    }
    vkCmdBindDescriptorSets(mCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderCommand.pipeline->GetPipelineLayout(), 0, renderCommand.descriptorCount, descriptorSets, 0, nullptr);

    renderCommand.pipeline->CmdBindPipeline(mCommandBuffer);

    vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
}

FrameInfo Renderer::mFrameInfo;
SampleCount Renderer::mSampleCount = SampleCount::Four;
glm::uvec2 Renderer::mResolution = glm::uvec2(1920, 1080);
GraphicsPipeline Renderer::mScenePipeline;
RenderPass Renderer::mSceneRenderPass;
FrameBuffer Renderer::mSceneFrameBuffer;
ImageDeprecated Renderer::mSceneColorAttachment;
ImageDeprecated Renderer::mSceneResolveAttachment;
CommandBuffer Renderer::mCommandBuffer;
RendererSpecification Renderer::mSpecification;
Semaphore Renderer::mImageAcquiredSemaphore;
Semaphore Renderer::mSwapchainRenderFinished;
GraphicsPipeline Renderer::mPresentPipeline;
RenderPass Renderer::mPresentRenderPass;
CommandBuffer Renderer::mPresentCommandBuffer;
Descriptor Renderer::mPresentInputDescriptor;
UniformBuffer Renderer::mUniformBuffer;
UniformData Renderer::mUniformData;
std::unordered_map<const Material *, RendererMaterialObject> Renderer::mMaterialObjectMap;
std::vector<RenderCommand> Renderer::mRenderCommands;
Camera Renderer::mCamera;
VertexShaderID Renderer::mBasicVertexShader = (VertexShaderID)UINT64_MAX;
FragmentShaderID Renderer::mBasicFragmentShader = (FragmentShaderID)UINT64_MAX;
ImageDeprecated Renderer::mSceneDepthAttachment;
ImageDeprecated Renderer::mSceneResolveDepthAttachment;
StorageBuffer Renderer::mLightStorageBuffer;
std::vector<LightUniformData> Renderer::mLight;
Sampler Renderer::mSampler;
Sampler Renderer::mDirectionalShadowSampler;
Descriptor Renderer::mShadowMapDescriptor;
std::vector<ImageDeprecated> Renderer::mShadowMaps;