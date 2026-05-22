#include "Renderer.hpp"
#include "Renderer/Helper.hpp"
#include <cstring>

void Renderer::Initialize(const Window &window)
{
    GraphicsContext::Initialize(window);

    mDefaultSampler.SetFilter(Filter::Nearest, Filter::Nearest);
    mDefaultSampler.Create();

    mSwapchain.CreateSwapchain(window.GetSize(), PresentMode::Fifo);

    CreateDeferredPassObjects();
    CreateLightingPassObjects();

    mRenderCommandBuffer.Create();
    mTransferToSwapchainCommandBuffer.Create();

    mImageAcquiredSemaphore.Create();
    mTransferSemaphore.Create();
}

void Renderer::Terminate()
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());
    mDeferred.uniformBuffer.Destroy();
    mDeferred.descriptor.Destroy();

    GraphicsContext::Terminate();
}

void Renderer::Submit(StaticMesh &mesh, Material &material)
{
    assert(mFrameInfo.isRecording == true);

    Submit(mesh, material, Transform());
}

void Renderer::Submit(StaticMesh &mesh, Material &material, const Transform &transform)
{
    assert(mFrameInfo.isRecording == true);

    RenderCommand renderCommand;
    renderCommand.vertexBuffer = &mesh.mVertexBuffer;
    renderCommand.indexBuffer = &mesh.mIndexBuffer;
    renderCommand.indexCount = mesh.mIndexSize / sizeof(uint32_t);
    renderCommand.pipeline = &material.GetPipelineRef();
    renderCommand.descriptors[0] = &material.GetImageDescriptorRef();
    renderCommand.descriptors[1] = &material.GetUniformDescriptorRef();
    renderCommand.descriptorCount = 2;

    if (material.IsInstancingEnabled())
    {
        renderCommand.instanceBuffer = &material.GetInstanceBufferRef();
        renderCommand.instanceCount = material.GetInstanceCount();
    }

    glm::mat4 matrix = transform.GetMatrix();
    memcpy((void *)renderCommand.pushContantData, &matrix, sizeof(matrix));
    renderCommand.pushContantSize = sizeof(matrix);

    Submit(renderCommand);
}

void Renderer::Submit(const RenderCommand &renderCommand)
{
    assert(mFrameInfo.isRecording == true);

    mRenderCommands.push_back(renderCommand);
}

void Renderer::BeginFrame(RenderTarget &renderTarget, const Camera &camera)
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    mFrameInfo.isRecording = true;
    mCurrentRenderTarget = renderTarget;

    renderTarget.TransitionLayout(ImageLayout::General);
    mLighting.descriptor.UpdateImage(renderTarget.GetImage(), ImageLayout::General, mDefaultSampler, 0);

    ResizeAttachments(renderTarget.GetImage().size);

    mFrameInfo.camera = camera;
    mDeferred.uniformData.cameraPosition = camera.GetPosition();
    mDeferred.uniformData.view = camera.GetView();
    mDeferred.uniformData.projection = camera.GetProjection();
    mDeferred.uniformData.projection[1][1] *= -1;

    mDeferred.uniformBuffer.SetData(&mDeferred.uniformData);

    mLighting.uniformData.cameraPosition = camera.GetPosition();
    mLighting.uniformBuffer.SetData(&mLighting.uniformData);
}

void Renderer::DeferredPass()
{
    mDeferred.renderPass.CmdBeginRenderPass(mRenderCommandBuffer, mDeferred.frameBuffer, mDeferred.attachment.size, {{0, 0, 0, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {1, 1, 1, 1}, {0, 0, 0, 1}});

    for (const RenderCommand &renderCommand : mRenderCommands)
    {
        uint32_t vertexBufferCount = (renderCommand.instanceCount == 0) ? 1 : 2;
        std::array<VkBuffer, 2> vertexBuffer = {renderCommand.vertexBuffer->handle};
        if (vertexBufferCount == 2)
        {
            vertexBuffer[1] = renderCommand.instanceBuffer->mBuffer.handle;
        }
        std::array<VkDeviceSize, 2> offsets = {0, 0};

        vkCmdBindVertexBuffers(mRenderCommandBuffer.GetHandle(), 0, vertexBufferCount, vertexBuffer.data(), offsets.data());
        vkCmdBindIndexBuffer(mRenderCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindPipeline(mRenderCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderCommand.pipeline->GetHandle());

        std::array<VkDescriptorSet, 16> descriptorSets = {};
        for (int j = 0; j < renderCommand.descriptorCount; j++)
        {
            descriptorSets[j] = renderCommand.descriptors[j]->GetDescriptorSet();
        }

        vkCmdBindDescriptorSets(mRenderCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderCommand.pipeline->GetPipelineLayout(), 0, renderCommand.descriptorCount, descriptorSets.data(), 0, nullptr);

        VkViewport viewport =
            {
                .width = (float)mDeferred.attachment.size.x,
                .height = (float)mDeferred.attachment.size.y,
                .minDepth = 0.f,
                .maxDepth = 1.f,
            };

        VkRect2D scissor =
            {
                .extent = {mDeferred.attachment.size.x, mDeferred.attachment.size.y},
            };

        vkCmdSetViewport(mRenderCommandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(mRenderCommandBuffer.GetHandle(), 0, 1, &scissor);

        vkCmdPushConstants(mRenderCommandBuffer.GetHandle(), renderCommand.pipeline->GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, renderCommand.pushContantSize, (void *)renderCommand.pushContantData);

        if (renderCommand.instanceCount == 0)
        {
            vkCmdDrawIndexed(mRenderCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDrawIndexed(mRenderCommandBuffer.GetHandle(), renderCommand.indexCount, renderCommand.instanceCount, 0, 0, 0);
        }
    }

    mDeferred.renderPass.CmdEndRenderPass(mRenderCommandBuffer);
}

void Renderer::LightingPass()
{
    CmdBindPipeline(mRenderCommandBuffer, mLighting.pipeline);
    CmdBindDescriptors(mRenderCommandBuffer, mLighting.pipeline, {mLighting.descriptor, mDeferred.descriptor, mLighting.uniformDescriptor});

    glm::uvec3 groupCount;
    groupCount.x = (mSwapchain.GetSize().x / 16) + 1;
    groupCount.y = (mSwapchain.GetSize().y / 16) + 1;
    groupCount.z = 1;
    vkCmdDispatch(mRenderCommandBuffer.GetHandle(), groupCount.x, groupCount.y, groupCount.z);
}

void Renderer::EndFrame()
{
    assert(mFrameInfo.isRecording == true);

    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    mRenderCommandBuffer.BeginRecording();

    DeferredPass();
    LightingPass();

    mRenderCommandBuffer.EndRecording();
    mRenderCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics, {}, mRenderingSemaphore, PipelineStage::ColorAttachmentOutput);

    mRenderCommands.clear();
    mFrameInfo.isRecording = false;
}

bool Renderer::ResizeSwapchain(const glm::uvec2 &size)
{
    if (mSwapchain.GetSize() == size)
    {
        return false;
    }

    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    mSwapchain.Destroy();
    mSwapchain.CreateSwapchain(size, PresentMode::Fifo);

    DestroyImage(mLighting.image);
    mLighting.image = CreateImage(mSwapchain.GetSize(), ImageFormat::RGBA8UNORM, ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
    TransitionImageLayout(ImageLayout::None, ImageLayout::General, ImageAspect::Color, mLighting.image);

    mLighting.descriptor.UpdateImage(mLighting.image, ImageLayout::General, mDefaultSampler, 0);

    mDeferred.descriptor.UpdateImage(mDeferred.attachment.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);

    return true;
}

void Renderer::DisplayToWindow(const RenderTarget &target)
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());
    uint32_t imageIndex = mSwapchain.GetNextImageIndex(mImageAcquiredSemaphore, {});
    std::array<VkSwapchainKHR, 8> swapchain = {mSwapchain.GetHandle()};

    mTransferToSwapchainCommandBuffer.BeginRecording();

    VkImageCopy region =
        {
            .srcSubresource =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .srcOffset = {},
            .dstSubresource =
                {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .dstOffset = {},
            .extent = {target.GetImage().size.x, target.GetImage().size.y, 1},
        };

    vkCmdCopyImage(mTransferToSwapchainCommandBuffer.GetHandle(), target.GetImage().handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, mSwapchain.GetImages()[imageIndex].handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    mTransferToSwapchainCommandBuffer.EndRecording();

    mTransferToSwapchainCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().transfer, mImageAcquiredSemaphore, mTransferSemaphore);

    std::array<VkSemaphore, 2> waitSemaphoreHandles = {mTransferSemaphore.GetHandle()};
    VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphoreHandles.data(),
            .swapchainCount = 1,
            .pSwapchains = swapchain.data(),
            .pImageIndices = &imageIndex,
        };

    vkQueuePresentKHR(GraphicsContext::GetQueues().present, &presentInfo);
}

const RenderPass &Renderer::GetDeferredRenderPass()
{
    return mDeferred.renderPass;
}

void Renderer::AddListener(const std::function<bool(uint32_t, void *)> &listener)
{
    mDispatcher.AddListener(listener);
}

void Renderer::QueueSwapchainResize(const glm::uvec2 &size)
{
    mSwapchainSize = size;
}

void Renderer::ResizeAttachments(const glm::uvec2 &size)
{
    if (size == mDeferred.attachment.albedo.size)
    {
        return;
    }

    vkDeviceWaitIdle(GraphicsContext::GetDevice());
    mDeferred.attachment.ResizeAttachment(size);
    mDeferred.frameBuffer.Destroy();
    mDispatcher.Dispatch((uint32_t)RendererEvent::DeferredAttachmentResize, &mDeferred.attachment);

    std::initializer_list<Image> attachments =
        {
            mDeferred.attachment.albedo,
            mDeferred.attachment.position,
            mDeferred.attachment.normal,
            mDeferred.attachment.depth,
        };

    mDeferred.frameBuffer = FrameBuffer(size, attachments, mDeferred.renderPass);

    mDeferred.descriptor.UpdateImage(mDeferred.attachment.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);
}

void Renderer::CreateDeferredPassObjects()
{
    mDeferred.descriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferred.descriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferred.descriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferred.descriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferred.descriptor.Create();

    mDeferred.attachment.CreateAttachment(mSwapchain.GetSize());

    mDeferred.descriptor.UpdateImage(mDeferred.attachment.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferred.descriptor.UpdateImage(mDeferred.attachment.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);

    mDeferred.renderPass.AddAttachment(ImageFormat::RGBA8, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferred.renderPass.AddAttachment(ImageFormat::RGBA32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferred.renderPass.AddAttachment(ImageFormat::RGBA32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferred.renderPass.AddAttachment(ImageFormat::D32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferred.renderPass.AddSubpass({0, 1, 2}, {}, 3, PipelineBindPoint::Graphic);
    mDeferred.renderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests);
    mDeferred.renderPass.CreateRenderPass();

    std::initializer_list<Image> attachments =
        {
            mDeferred.attachment.albedo,
            mDeferred.attachment.position,
            mDeferred.attachment.normal,
            mDeferred.attachment.depth,
        };

    mDeferred.frameBuffer = FrameBuffer(mSwapchain.GetSize(), attachments, mDeferred.renderPass);

    mDeferred.uniformBuffer = UniformBuffer(sizeof(mDeferred.uniformData));
    mDeferred.uniformBuffer.SetData(&mDeferred.uniformData);
}

void Renderer::CreateLightingPassObjects()
{
    mLighting.image = CreateImage(mSwapchain.GetSize(), ImageFormat::RGBA8UNORM, ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
    TransitionImageLayout(ImageLayout::None, ImageLayout::General, ImageAspect::Color, mLighting.image);

    mLighting.descriptor.AddDescriptor(DescriptorType::StorageImage, ShaderStage::Compute);
    mLighting.descriptor.Create();
    mLighting.descriptor.UpdateImage(mLighting.image, ImageLayout::General, mDefaultSampler, 0);

    mLighting.uniformBuffer = UniformBuffer(sizeof(mLighting.uniformData));

    mLighting.uniformDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Compute);
    mLighting.uniformDescriptor.Create();
    mLighting.uniformDescriptor.UpdateBuffer(mLighting.uniformBuffer.GetBuffer(), 0);

    mLighting.pipeline.LoadShader("Shaders/swapchain.comp.spv");
    mLighting.pipeline.Create({&mLighting.descriptor, &mDeferred.descriptor, &mLighting.uniformDescriptor});
}

glm::uvec2 Renderer::mSwapchainSize = {};
EventDispatcher Renderer::mDispatcher = {};
GraphicsContext Renderer::mContext = {};
Swapchain Renderer::mSwapchain = {};
std::vector<RenderCommand> Renderer::mRenderCommands = {};
RenderTarget Renderer::mCurrentRenderTarget = {};
CommandBuffer Renderer::mRenderCommandBuffer = {};
CommandBuffer Renderer::mTransferToSwapchainCommandBuffer = {};
Semaphore Renderer::mImageAcquiredSemaphore = {};
Semaphore Renderer::mTransferSemaphore = {};
Semaphore Renderer::mRenderingSemaphore = {};
Sampler Renderer::mDefaultSampler = {};
FrameInfo Renderer::mFrameInfo = {};
Renderer::Lighting Renderer::mLighting = {};
Renderer::Deferred Renderer::mDeferred = {};
VertexShaderID Renderer::mBasicVertexShader = (VertexShaderID)UINT64_MAX;
FragmentShaderID Renderer::mBasicFragmentShader = (FragmentShaderID)UINT64_MAX;