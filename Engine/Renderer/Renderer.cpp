#include "Renderer.hpp"
#include "Assets/ShaderManager.hpp"
#include "Core/Application.hpp"
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
    mUniformBuffer.SetData(&mUniformData);
    mLightStorageBuffer.CreateStorageBuffer(nullptr, sizeof(Light) * maxLightCount);

    mBufferDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    mBufferDescriptor.AddDescriptor(DescriptorType::StorageBuffer, ShaderStage::Fragment);
    mBufferDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Fragment);
    mBufferDescriptor.CreateDescriptor();
    mBufferDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);
    mBufferDescriptor.UpdateBuffer(mLightStorageBuffer.GetBuffer(), 1);
}

void Renderer::Terminate()
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    TextureManager::Terminate();

    DestroyImage(mSceneColorAttachment);
    DestroyImage(mSceneResolveAttachment);
    mSceneFrameBuffer.DestroyFrameBuffer();
    mSceneRenderPass.DestroyRenderPass();
    mCommandBuffer.DestroyCommandBuffer();
    mPresentCommandBuffer.DestroyCommandBuffer();
    mPresentInputDescriptor.DestroyDescriptor();
    mPresentPipeline.DestroyPipeline();
    mPresentRenderPass.DestroyRenderPass();

    mUniformBuffer.DestroyUniformBuffer();

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
    mUniformData.time = Application::GetInstance()->GetElapsedTime();
    mUniformBuffer.SetData(&mUniformData);

    mBufferDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);
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
    vkCmdSetCullMode(mPresentCommandBuffer.GetHandle(), VK_CULL_MODE_NONE);
    vkCmdSetDepthTestEnable(mPresentCommandBuffer.GetHandle(), false);
    vkCmdSetDepthWriteEnable(mPresentCommandBuffer.GetHandle(), false);

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
    renderCommand.descriptorCount = 3;
    renderCommand.descriptors[0] = &TextureManager::GetDescriptor();
    renderCommand.descriptors[1] = &mBufferDescriptor;
    renderCommand.descriptors[2] = &mShadowMapDescriptor;
    renderCommand.pipeline = &mShaderPipelineMap[material.shader];
    renderCommand.indexCount = mesh.mIndexSize / sizeof(uint32_t);

    renderCommand.pipelineSettings.cullMode = material.cullMode;
    renderCommand.pipelineSettings.enableDepthTest = material.enableDepthTest;
    renderCommand.pipelineSettings.enableDepthWrite = material.enableDepthWrite;

    PushConstantData data;
    data.model = transform.GetMatrix();
    data.albedoIndex = (uint32_t)material.albedo;
    data.roughnessIndex = (uint32_t)material.roughness;
    data.metallicIndex = (uint32_t)material.metallic;
    data.normalIndex = (uint32_t)material.normal;
    data.inputInt = mInputInt;
    data.roughness = material.roughnessFactor;
    data.metallic = material.metallicFactor;
    data.indexOfRefraction = material.indexOfRefraction;

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
    mBasicShaderID = ShaderManager2::Load(vertexShader, fragmentShader);
}

ShaderID Renderer::GetBasicShaderID()
{
    return mBasicShaderID;
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
        mShadowMapDescriptor.UpdateImageIndex(mShadowMaps[i], ImageLayout::ShaderRead, mSampler, 0, i);
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

void Renderer::CreateGraphicsPipeline(ShaderID id)
{
    const Shader &shader = ShaderManager2::Get(id);

    GraphicsPipeline pipeline;
    pipeline.SetVertexShader(shader.vertex);
    pipeline.SetFragmentShader(shader.fragment);
    if (shader.vertex != VK_NULL_HANDLE)
    {
        pipeline.SetGeometryShader(shader.geometry);
    }

    pipeline.AddDescriptors(TextureManager::GetDescriptor());
    pipeline.AddDescriptors(mBufferDescriptor);
    pipeline.AddDescriptors(mShadowMapDescriptor);
    pipeline.SetCullMode(CullMode::Back);
    pipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    pipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    pipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    pipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));
    pipeline.AddAttribute(0, 3, ImageFormat::RGB32, offsetof(Vertex, tangent));
    pipeline.AddAttribute(0, 4, ImageFormat::RGB32, offsetof(Vertex, bitangent));
    pipeline.EnableDepthWrite(true);
    pipeline.EnableDepthTesting(true);
    pipeline.SetMultisampleCount(mSampleCount);
    pipeline.SetPushConstant(ShaderStage::All, sizeof(PushConstantData));
    pipeline.AddColorBlendAttachment(true);
    pipeline.CreatePipeline(mSceneRenderPass, 0);

    mShaderPipelineMap[id] = pipeline;
}
VkRenderPass Renderer::GetRenderPass()
{
    return mSceneRenderPass.GetHandle();
}
const RenderPass &Renderer::GetPresentRenderPass()
{
    return mPresentRenderPass;
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

void Renderer::CreatePresentPipeline()
{

    ShaderID fullscreenShader = ShaderManager2::Load("Shaders/fullscreen.vert.spv", "Shaders/fullscreen.frag.spv", "", "", false);

    mPresentPipeline.AddDescriptors(mPresentInputDescriptor);

    mPresentPipeline.AddColorBlendAttachment(false);

    mPresentPipeline.SetVertexShader(ShaderManager2::Get(fullscreenShader).vertex);
    mPresentPipeline.SetFragmentShader(ShaderManager2::Get(fullscreenShader).fragment);

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
    vkCmdSetCullMode(mCommandBuffer.GetHandle(), GetVulkanCullMode(renderCommand.pipelineSettings.cullMode));
    vkCmdSetDepthTestEnable(mCommandBuffer.GetHandle(), (VkBool32)renderCommand.pipelineSettings.enableDepthTest);
    vkCmdSetDepthWriteEnable(mCommandBuffer.GetHandle(), (VkBool32)renderCommand.pipelineSettings.enableDepthWrite);

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
// GraphicsPipeline Renderer::mScenePipeline;
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
std::vector<RenderCommand> Renderer::mRenderCommands;
Camera Renderer::mCamera;
ImageDeprecated Renderer::mSceneDepthAttachment;
ImageDeprecated Renderer::mSceneResolveDepthAttachment;
StorageBuffer Renderer::mLightStorageBuffer;
std::vector<LightUniformData> Renderer::mLight;
Sampler Renderer::mSampler;
Descriptor Renderer::mShadowMapDescriptor;
std::vector<ImageDeprecated> Renderer::mShadowMaps;
Descriptor Renderer::mBufferDescriptor;
ShaderID Renderer::mBasicShaderID;
std::unordered_map<ShaderID, GraphicsPipeline> Renderer::mShaderPipelineMap;
uint32_t Renderer::mInputInt;