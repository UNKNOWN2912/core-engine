#include "Renderer.hpp"
#include "Assets/ShaderManager.hpp"
#include "Core/Application.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <cstring>

#define USE_DEPTH_PREPASS 0

void Renderer::Initialize(const RendererSpecification &specification)
{
    mSpecification = specification;

    mTextureDescriptor = std::move(Descriptor(std::initializer_list<DescriptorLayout>{{DescriptorType::CombinedSampler, ShaderStage::Fragment, 1024}}));
    mShadowMapDescriptor = std::move(Descriptor(std::initializer_list<DescriptorLayout>{{DescriptorType::CombinedSampler, ShaderStage::Fragment, 1024}}));

    mCommandBuffer = CommandBuffer(GraphicsContext::GetCurrentContext().GetCommandPool());
    mPresentCommandBuffer = CommandBuffer(GraphicsContext::GetCurrentContext().GetCommandPool());

    if (mSampleCount != SampleCount::One)
    {
        CreateSceneRenderPassMultisampled();
        CreateSceneAttachmentsMultisampled();
        CreateSceneFrameBufferMultisampled();
    }
    else
    {
        CreateSceneRenderPass();
        CreateSceneAttachments();
        CreateSceneFrameBuffer();
    }

    mSampler = Sampler(Filter::Linear, Filter::Linear, AddressMode::Repeat, AddressMode::Repeat, AddressMode::Repeat, true, CompareType::Less);

    mPresentInputDescriptor = Descriptor(std::initializer_list<DescriptorLayout>{
        {DescriptorType::CombinedSampler, ShaderStage::Fragment},
        {DescriptorType::CombinedSampler, ShaderStage::Fragment},
        {DescriptorType::StorageImage, ShaderStage::Fragment},
    });

    mPresentInputDescriptor.UpdateImage(mSceneResolveAttachment, ImageLayout::ShaderRead, mSampler, 0);
    mPresentInputDescriptor.UpdateImage(mSceneResolveDepthAttachment, ImageLayout::ShaderRead, mSampler, 1);

    CreatePresentRenderPass();
    CreatePresentPipeline();
    mImageAcquiredSemaphore.CreateSemaphore();
    mSwapchainRenderFinished.CreateSemaphore();

    mLight.reserve(1000);

    mUniformBuffer = UniformBuffer(sizeof(UniformData));
    mUniformBuffer.SetData(&mUniformData);
    mLightStorageBuffer = StorageBuffer(sizeof(Light) * maxLightCount);

    mBufferDescriptor = std::move(Descriptor(std::initializer_list<DescriptorLayout>{
        {DescriptorType::Uniform, ShaderStage::Vertex},
        {DescriptorType::StorageBuffer, ShaderStage::Fragment},
        {DescriptorType::Uniform, ShaderStage::Fragment},
    }));
    mBufferDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);
    mBufferDescriptor.UpdateBuffer(mLightStorageBuffer.GetBuffer(), 1);

    mViewportSize = mResolution;

#if USE_DEPTH_PREPASS
    mDepthPrepassShader.AddDescriptor(mBufferDescriptor);
    mDepthPrepassShader.AddLayout(Vertex::GetVertexLayout(0, 0));
    mDepthPrepassShader.SetPushConstantSize(sizeof(PushConstantData));
    mDepthPrepassShader.GetSettings().sampleCount = Renderer::GetSampleCount();
    mDepthPrepassShader.GetSettings().cullMode = CullMode::Back;
    mDepthPrepassShader.Load("Shaders/prepass.vert.spv", "Shaders/prepass.frag.spv", Renderer::GetRenderPass(), 0);

#endif
}

void Renderer::Terminate()
{
    vkDeviceWaitIdle(GraphicsContext::GetCurrentContext().GetDevice());
    DestroyImage(mSceneColorAttachment);
    DestroyImage(mSceneResolveAttachment);
    mSceneRenderPass.DestroyRenderPass();
    mPresentInputDescriptor.Destroy();
    mPresentRenderPass.DestroyRenderPass();
}

Renderer::Renderer(const RendererSpecification &specification)
{
    Initialize(specification);
}

Renderer::~Renderer()
{
    Terminate();
}

Renderer &Renderer::operator=(Renderer &&renderer)
{
    Terminate();
    Move(std::move(renderer));
    return *this;
}

Renderer::Renderer(Renderer &&renderer)
{
    Move(std::move(renderer));
}

void Renderer::Move(Renderer &&renderer)
{
    mInputInt = renderer.mInputInt;
    mTextureDescriptor = std::move(renderer.mTextureDescriptor);
    mBufferDescriptor = std::move(renderer.mBufferDescriptor);
    mSampler = std::move(renderer.mSampler);
    mFrameInfo = renderer.mFrameInfo;
    mSpecification = renderer.mSpecification;
    mSampleCount = renderer.mSampleCount;
    mResolution = renderer.mResolution;
    mSceneRenderPass = std::move(renderer.mSceneRenderPass);
    mSceneFrameBuffer = std::move(renderer.mSceneFrameBuffer);
    mSceneColorAttachment = (renderer.mSceneColorAttachment);
    mSceneResolveAttachment = (renderer.mSceneResolveAttachment);
    mSceneDepthAttachment = (renderer.mSceneDepthAttachment);
    mSceneResolveDepthAttachment = (renderer.mSceneResolveDepthAttachment);
    mCommandBuffer = std::move(renderer.mCommandBuffer);
    mImageAcquiredSemaphore = renderer.mImageAcquiredSemaphore;
    mSwapchainRenderFinished = renderer.mSwapchainRenderFinished;
    mPresentShader = std::move(renderer.mPresentShader);
    mPresentRenderPass = std::move(renderer.mPresentRenderPass);
    mPresentCommandBuffer = std::move(renderer.mPresentCommandBuffer);
    mPresentInputDescriptor = std::move(renderer.mPresentInputDescriptor);
    mUniformBuffer = std::move(renderer.mUniformBuffer);
    mUniformData = renderer.mUniformData;
    mRenderCommands = renderer.mRenderCommands;
    mShadowMaps = std::move(renderer.mShadowMaps);
    mLight = std::move(renderer.mLight);
    mLightStorageBuffer = std::move(renderer.mLightStorageBuffer);
    mShadowMapDescriptor = std::move(renderer.mShadowMapDescriptor);
    mViewportSize = renderer.mViewportSize;
    mDepthPrepassShader = std::move(renderer.mDepthPrepassShader);
    mInputInt = renderer.mInputInt;

    renderer.mFrameInfo = {};
    renderer.mSpecification = {};
    renderer.mSampleCount = {};
    renderer.mResolution = {};
    renderer.mSceneColorAttachment = {};
    renderer.mSceneResolveAttachment = {};
    renderer.mSceneDepthAttachment = {};
    renderer.mSceneResolveDepthAttachment = {};
    renderer.mImageAcquiredSemaphore = {};
    renderer.mSwapchainRenderFinished = {};
    renderer.mUniformData = {};
    renderer.mViewportSize = {};
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

void Renderer::EndFrame(const glm::vec4 &clearColor)
{
    assert(mFrameInfo.recording);
    mFrameInfo = FrameInfo();

    mCommandBuffer.BeginRecording();

    VkClearValue vkClearColor = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
    mSceneRenderPass.CmdBeginRenderPass(mCommandBuffer, mSceneFrameBuffer, mResolution, {vkClearColor, vkClearColor, {1, 1, 1, 1}, {1, 1, 1, 1}});

#if USE_DEPTH_PREPASS
    for (const RenderCommand &renderCommand : mRenderCommands)
    {
        mDepthPrepassShader.GetGraphicsPipeline().CmdBindPipeline(mCommandBuffer);

        uint32_t vertexBufferCount = 1;
        VkBuffer vertexBuffer[2] = {renderCommand.vertexBuffer->handle};
        if (renderCommand.instanceBuffer != nullptr)
        {
            vertexBuffer[1] = renderCommand.instanceBuffer->GetBuffer().handle;
            vertexBufferCount = 2;
        }

        VkDeviceSize offsets[] = {0, 0};

        vkCmdBindVertexBuffers(mCommandBuffer.GetHandle(), 0, vertexBufferCount, vertexBuffer, offsets);

        vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);

        VkViewport viewport =
            {
                .width = (float)mViewportSize.x,
                .height = (float)mViewportSize.y,
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

        if (renderCommand.pushContantSize != 0)
        {
            vkCmdPushConstants(mCommandBuffer.GetHandle(), renderCommand.pipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, renderCommand.pushContantSize, renderCommand.pushContantData);
        }

        VkDescriptorSet descriptorSets[1] = {mBufferDescriptor.GetDescriptorSet()};
        vkCmdBindDescriptorSets(mCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mDepthPrepassShader.GetGraphicsPipeline().GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

        vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, renderCommand.instanceCount, 0, 0, 0);
    }

    vkCmdNextSubpass(mCommandBuffer.GetHandle(), VK_SUBPASS_CONTENTS_INLINE);
#endif

    RenderCommand mPreviousCommand;

    for (const RenderCommand &renderCommand : mRenderCommands)
    {
        CmdDrawRenderCommand(renderCommand, mPreviousCommand);
        mPreviousCommand = renderCommand;
    }

    mSceneRenderPass.CmdEndRenderPass(mCommandBuffer);

    mCommandBuffer.EndRecording();

    mCommandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().graphics);
}
void Renderer::SetResolution(const glm::uvec2 &resolution)
{
    mResolution = resolution;
}
const glm::uvec2 &Renderer::GetResolution() const
{
    return mResolution;
}
SampleCount Renderer::GetSampleCount() const
{
    return Renderer::mSampleCount;
}

void Renderer::SetSampleCount(const SampleCount &sampleCount)
{
    Renderer::mSampleCount = sampleCount;
};

Surface Renderer::CreateSurface(const Window &window, ImageFormat format)
{
    Surface surface;
    surface.handle = window.CreateWindowSurface();
    surface.swapchain.CreateSwapchain(surface.handle, format, ColorSpace::SRGBNonLinear, PresentMode::Fifo);

    for (const ImageDeprecated &image : surface.swapchain.GetImages())
    {
        surface.frameBuffers.emplace_back(image.size, std::vector<VkImageView>{image.view}, mPresentRenderPass);
    }

    return surface;
}

void Renderer::ResizeSurface(Surface &surface, ImageFormat format)
{
    vkDeviceWaitIdle(GraphicsContext::GetCurrentContext().GetDevice());

    surface.frameBuffers.clear();

    surface.swapchain.DestroySwapchain();

    surface.swapchain.CreateSwapchain(surface.handle, format, ColorSpace::SRGBNonLinear, PresentMode::Fifo);

    for (const ImageDeprecated &image : surface.swapchain.GetImages())
    {
        surface.frameBuffers.emplace_back(image.size, std::vector<VkImageView>{image.view}, mPresentRenderPass);
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
    mPresentRenderPass.CmdBeginRenderPass(mPresentCommandBuffer, surface.frameBuffers[imageIndex], surface.swapchain.GetSize(), {{0, 0, 0, 0}});

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
    vkCmdBindDescriptorSets(mPresentCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mPresentShader.GetGraphicsPipeline().GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);

    mPresentShader.GetGraphicsPipeline().CmdBindPipeline(mPresentCommandBuffer);

    vkCmdDraw(mPresentCommandBuffer.GetHandle(), 6, 1, 0, 0);

    mPresentRenderPass.CmdEndRenderPass(mPresentCommandBuffer);
    mPresentCommandBuffer.EndRecording();

    mPresentCommandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().graphics, mImageAcquiredSemaphore, mSwapchainRenderFinished, PipelineStage::ColorAttachmentOutput);

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

    vkQueuePresentKHR(GraphicsContext::GetCurrentContext().GetQueues().graphics, &presentInfo);
}

ShaderConfig Renderer::GetSceneShaderConfig() const
{

    ShaderConfig config =
        {
            .sampleCount = GetSampleCount(),
            .cullMode = CullMode::Back,
            .enableDepthWrite = true,
            .enableDepthTest = true,
            .descriptors = {GetTextureDescriptor(), GetBufferDescriptor(), GetShadowMapDescriptor()},
            .colorBlendAttachments = {false},
            .layouts = {Vertex::GetVertexLayout(0, 0)},
            .pushConstantSize = sizeof(PushConstantData),
        };

#if USE_DEPTH_PREPASS
    config.compare = CompareType::Equal;
    config.enableDepthWrite = false;
#endif

    return config;
}

const std::vector<RenderCommand> &Renderer::GetRenderCommands() const
{
    return mRenderCommands;
}

void Renderer::Submit(RenderCommand renderCommand)
{
    mRenderCommands.push_back(renderCommand);
}

void Renderer::Submit(const Mesh &mesh, const Material &material, const Transform &transform, const TextureManager &textureManager, const ShaderManager &shaderManager)
{
    RenderCommand renderCommand;
    renderCommand.vertexBuffer = &mesh.GetVertexBuffer();
    renderCommand.indexBuffer = &mesh.GetIndexBuffer();
    renderCommand.descriptorCount = 3;
    renderCommand.descriptors[0] = &mTextureDescriptor;
    renderCommand.descriptors[1] = &mBufferDescriptor;
    renderCommand.descriptors[2] = &mShadowMapDescriptor;
    renderCommand.pipeline = &shaderManager.Get(material.shader).GetGraphicsPipeline();
    renderCommand.indexCount = mesh.mIndexSize / sizeof(uint32_t);

    renderCommand.pipelineSettings.cullMode = material.cullMode;
    renderCommand.pipelineSettings.enableDepthTest = material.enableDepthTest;
    renderCommand.pipelineSettings.enableDepthWrite = material.enableDepthWrite;

    PushConstantData data;
    data.model = transform.GetMatrix();
    data.albedoIndex = textureManager.GetTextureDescriptorIndex(material.albedoTexture);
    data.roughnessIndex = textureManager.GetTextureDescriptorIndex(material.roughnessTexture);
    data.metallicIndex = textureManager.GetTextureDescriptorIndex(material.metallicTexture);
    data.normalIndex = textureManager.GetTextureDescriptorIndex(material.normalTexture);
    data.inputInt = mInputInt;
    data.roughness = material.roughnessFactor;
    data.metallic = material.metallicFactor;
    data.indexOfRefraction = material.indexOfRefraction;
    data.color = material.colorFactor;

    memcpy(renderCommand.pushContantData, &data, sizeof(data));
    renderCommand.pushContantSize = sizeof(data);

    renderCommand.debugName = "Mesh Material";

    mRenderCommands.push_back(renderCommand);
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

void Renderer::CreateGraphicsPipeline(std::string_view identifier, ShaderManager &shaderManager)
{
    const Shader &shader = shaderManager.Get(identifier);
}
uint32_t Renderer::GetInputInt() const
{
    return mInputInt;
}
void Renderer::SetInputInt(uint32_t inputInt)
{
    mInputInt = inputInt;
}
RenderPass &Renderer::GetRenderPass()
{
    return mSceneRenderPass;
}
const RenderPass &Renderer::GetRenderPass() const
{
    return mSceneRenderPass;
}
const RenderPass &Renderer::GetPresentRenderPass() const
{
    return mPresentRenderPass;
}
const glm::uvec2 &Renderer::GetViewportSize() const
{
    return mViewportSize;
}

void Renderer::SetViewportSize(const glm::uvec2 &size)
{
    mViewportSize = size;
}

uint32_t Renderer::GetRenderPassColorSubpassIndex() const
{

    uint32_t index = 0;

#if USE_DEPTH_PREPASS
    index = 1;
#endif

    return index;
}

void Renderer::CreateSceneRenderPassMultisampled()
{
    uint32_t colorResolve = mSceneRenderPass.AddAttachment(mSpecification.presentationFormat, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    uint32_t colorAttachment = mSceneRenderPass.AddAttachment(mSpecification.presentationFormat, ImageLayout::None, ImageLayout::ColorAttachment, LoadOperation::Clear, StoreOperation::DontCare, LoadOperation::DontCare, StoreOperation::DontCare, mSampleCount);
    uint32_t depthResolve = mSceneRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    uint32_t depthAttachment = mSceneRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::DepthStencil, LoadOperation::Clear, StoreOperation::DontCare, LoadOperation::DontCare, StoreOperation::DontCare, mSampleCount);

#if USE_DEPTH_PREPASS

    Subpass depthPass;
    depthPass.SetDepthAttachment(depthAttachment);
    depthPass.SetDepthResolveAttachment(depthResolve);

    Subpass subpass;
    subpass.AddColorAttachment(colorAttachment);
    subpass.AddResolveAttachment(colorResolve);
    subpass.SetDepthAttachment(depthAttachment);

    mSceneRenderPass.AddSubpass(depthPass, PipelineBindPoint::Graphic);
    mSceneRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);

    mSceneRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mSceneRenderPass.AddDependency(0, 1, PipelineStage::EarlyFragmentTests, PipelineStage::LateFragmentTests);
#else

    Subpass subpass;
    subpass.AddColorAttachment(colorAttachment);
    subpass.AddResolveAttachment(colorResolve);
    subpass.SetDepthAttachment(depthAttachment);
    subpass.SetDepthResolveAttachment(depthResolve);

    mSceneRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);

    mSceneRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
#endif

    mSceneRenderPass.CreateRenderPass();
}

void Renderer::CreateSceneFrameBufferMultisampled()
{
    mSceneFrameBuffer = FrameBuffer(mSceneResolveAttachment.size, std::vector<VkImageView>{mSceneResolveAttachment.view, mSceneColorAttachment.view, mSceneResolveDepthAttachment.view, mSceneDepthAttachment.view}, mSceneRenderPass);
}

void Renderer::CreateSceneAttachmentsMultisampled()
{
    mSceneColorAttachment = CreateImage(mResolution, mSpecification.presentationFormat, ImageUsage::ColorAttachment, ImageAspect::Color, MemoryProperty::DeviceLocal, mSampleCount);
    mSceneResolveAttachment = CreateImage(mResolution, mSpecification.presentationFormat, ImageUsage::ColorAttachment | ImageUsage::Sampler | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal, SampleCount::One);
    mSceneDepthAttachment = CreateImage(mResolution, ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal, mSampleCount);
    mSceneResolveDepthAttachment = CreateImage(mResolution, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
}

void Renderer::CreateSceneRenderPass()
{
    uint32_t colorResolve = mSceneRenderPass.AddAttachment(mSpecification.presentationFormat, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    uint32_t depthResolve = mSceneRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);

    Subpass subpass;
    subpass.AddColorAttachment(colorResolve);
    subpass.SetDepthAttachment(depthResolve);

    mSceneRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);

    mSceneRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);

    mSceneRenderPass.CreateRenderPass();
}

void Renderer::CreateSceneFrameBuffer()
{
    mSceneFrameBuffer = FrameBuffer(mSceneResolveAttachment.size, std::vector<VkImageView>{mSceneResolveAttachment.view, mSceneResolveDepthAttachment.view}, mSceneRenderPass);
}

void Renderer::CreateSceneAttachments()
{
    mSceneResolveAttachment = CreateImage(mResolution, mSpecification.presentationFormat, ImageUsage::ColorAttachment | ImageUsage::Sampler | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal, SampleCount::One);
    mSceneResolveDepthAttachment = CreateImage(mResolution, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
}

void Renderer::CreatePresentPipeline()
{
    ShaderConfig config =
        {
            .cullMode = CullMode::None,
            .descriptors = {mPresentInputDescriptor},
            .colorBlendAttachments = {false},

        };

    mPresentShader = Shader("Shaders/fullscreen.vert.spv", "Shaders/fullscreen.frag.spv", mPresentRenderPass, 0, config);
}

void Renderer::CreatePresentRenderPass()
{
    uint32_t attachmentIndex = mPresentRenderPass.AddAttachment(mSpecification.presentationFormat, ImageLayout::None, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);

    Subpass subpass;
    subpass.AddColorAttachment(attachmentIndex);
    mPresentRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);
    mPresentRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mPresentRenderPass.CreateRenderPass();
}

void Renderer::CmdDrawRenderCommand(const RenderCommand &renderCommand, const RenderCommand &previousCommand)
{
    if (renderCommand.pipeline != previousCommand.pipeline)
    {
        renderCommand.pipeline->CmdBindPipeline(mCommandBuffer);
    }

    uint32_t vertexBufferCount = 1;
    VkBuffer vertexBuffer[2] = {renderCommand.vertexBuffer->handle};
    if (renderCommand.instanceBuffer != nullptr)
    {
        vertexBuffer[1] = renderCommand.instanceBuffer->GetBuffer().handle;
        vertexBufferCount = 2;
    }

    VkDeviceSize offsets[] = {0, 0};

    if (renderCommand.vertexBuffer != previousCommand.vertexBuffer || renderCommand.instanceBuffer != previousCommand.instanceBuffer)
    {
        vkCmdBindVertexBuffers(mCommandBuffer.GetHandle(), 0, vertexBufferCount, vertexBuffer, offsets);
    }

    if (renderCommand.indexBuffer != previousCommand.indexBuffer)
    {
        vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);
    }

    VkViewport viewport =
        {
            .width = (float)mViewportSize.x,
            .height = (float)mViewportSize.y,
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

    if (renderCommand.pushContantSize != 0)
    {
        vkCmdPushConstants(mCommandBuffer.GetHandle(), renderCommand.pipeline->GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, renderCommand.pushContantSize, renderCommand.pushContantData);
    }

    bool descriptorChanged = false;
    VkDescriptorSet descriptorSets[32];
    for (int i = 0; i < renderCommand.descriptorCount; i++)
    {
        descriptorSets[i] = renderCommand.descriptors[i]->GetDescriptorSet();
        if (renderCommand.descriptors[i] != previousCommand.descriptors[i])
        {
            descriptorChanged = true;
        }
    }
    if (descriptorChanged)
    {
        vkCmdBindDescriptorSets(mCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderCommand.pipeline->GetPipelineLayout(), 0, renderCommand.descriptorCount, descriptorSets, 0, nullptr);
    }

    vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, renderCommand.instanceCount, 0, 0, 0);
}
