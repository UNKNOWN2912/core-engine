#include "Renderer.hpp"

void Renderer::Initialize(const Window& window)
{
    mContext.Create(window, true);

    mDefaultSampler.Create();

    mSwapchain.Create(window.GetSize(), PresentMode::Fifo);
    CreateAttachments(mSwapchain.GetSize());


    mComputeImage = CreateImage(mSwapchain.GetSize(), ImageFormat::RGBA8UNORM, ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
    TransitionImageLayout(ImageLayout::None, ImageLayout::General, ImageAspect::Color, mComputeImage);


    mComputeDescriptor.AddDescriptor(DescriptorType::StorageImage, ShaderStage::Compute);
    mComputeDescriptor.Create();

    mComputeDescriptor.UpdateImage(mComputeImage, ImageLayout::General, mDefaultSampler, 0);


    mDeferredAttachmentDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferredAttachmentDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferredAttachmentDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferredAttachmentDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Compute);
    mDeferredAttachmentDescriptor.Create();

    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);

    mComputePipeline.LoadShader("Shaders/swapchain.comp.spv");
    mComputePipeline.Create({mComputeDescriptor, mDeferredAttachmentDescriptor});

    // Render pass
    CreateDeferredRenderPass();

    // Attachments

    // Frame buffer
    CreateDeferredFrameBuffer(mSwapchain.GetSize());

    
    mRenderCommandBuffer.Create();
    mTransferToSwapchainCommandBuffer.Create();

    mImageAcquiredSemaphore.Create();
    mTransferSemaphore.Create();
}

void Renderer::Terminate() 
{
    
}

void Renderer::Submit(const RenderCommand& renderCommand) 
{
    assert(mFrameRecording == true);

    mRenderCommands.push_back(renderCommand);    
}

void Renderer::BeginFrame(RenderTarget& renderTarget) 
{
    vkDeviceWaitIdle(getDevice());

    mFrameRecording = true;    
    mCurrentRenderTarget = renderTarget;

    renderTarget.TransitionLayout(ImageLayout::General);
    mComputeDescriptor.UpdateImage(renderTarget.GetImage(), ImageLayout::General, mDefaultSampler, 0);

    ResizeAttachments(renderTarget.GetImage().size);
}

void Renderer::EndFrame() 
{
    assert(mFrameRecording == true);

    vkDeviceWaitIdle(getDevice());

    mRenderCommandBuffer.BeginRecording();

    // Begin Deferred render pass

    mDeferredRenderPass.CmdBeginRenderPass(mRenderCommandBuffer, mDeferredFrameBuffer, mDeferredAttachments.size, {{0,0,0,1}, {0,0,0,1}, {0,0,0,1}, {1,1,1,1}, {0,0,0,1}});

    for (int i = 0; i < mRenderCommands.size(); i++)
    {
        const RenderCommand& renderCommand = mRenderCommands[i];

        VkBuffer vertexBuffer[] = {renderCommand.vertexBuffer.handle};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(mRenderCommandBuffer.GetHandle(), 0, 1, vertexBuffer, offsets);
        vkCmdBindIndexBuffer(mRenderCommandBuffer.GetHandle(), renderCommand.indexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindPipeline(mRenderCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, renderCommand.pipeline.GetHandle());

        VkViewport viewport = 
        {
            .width = (float)mDeferredAttachments.size.x,
            .height = (float)mDeferredAttachments.size.y,
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };

        VkRect2D scissor = 
        {
            .extent = {mDeferredAttachments.size.x, mDeferredAttachments.size.y},
        };

        vkCmdSetViewport(mRenderCommandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(mRenderCommandBuffer.GetHandle(), 0, 1, &scissor);

        vkCmdDrawIndexed(mRenderCommandBuffer.GetHandle(), 3, 1, 0, 0, 0);
    }   

    mDeferredRenderPass.CmdEndRenderPass(mRenderCommandBuffer);

    // End Deferred render pass

    VkDescriptorSet descriptorSets[] = {mComputeDescriptor.GetDescriptorSet(), mDeferredAttachmentDescriptor.GetDescriptorSet()};
    vkCmdBindDescriptorSets(mRenderCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline.GetPipelineLayout(), 0, sizeof(descriptorSets) / sizeof(VkDescriptorSet), descriptorSets, 0, nullptr);
    vkCmdBindPipeline(mRenderCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_COMPUTE, mComputePipeline.GetHandle());

    glm::ivec3 groupCount;
    groupCount.x = (mSwapchain.GetSize().x / 16) + 1;
    groupCount.y = (mSwapchain.GetSize().y / 16) + 1;
    groupCount.z = 1;
    vkCmdDispatch(mRenderCommandBuffer.GetHandle(), groupCount.x, groupCount.y, groupCount.z);

    mRenderCommandBuffer.EndRecording();
    mRenderCommandBuffer.QueueSubmit(getQueues().graphics, {}, mRenderingSemaphore, PipelineStage::ColorAttachmentOutput);

    mRenderCommands.clear();
    mFrameRecording = false;    

}

void Renderer::ResizeSwapchain(const glm::uvec2& size) 
{
    if(mSwapchain.GetSize() == size)
        return;
    
    vkDeviceWaitIdle(getDevice());

    mSwapchain.Destroy();
    mSwapchain.Create(size, PresentMode::Fifo);

    DestroyImage(mComputeImage);
    mComputeImage = CreateImage(mSwapchain.GetSize(), ImageFormat::RGBA8UNORM, ImageUsage::Storage | ImageUsage::TransferSource, ImageAspect::Color, MemoryProperty::DeviceLocal);
    TransitionImageLayout(ImageLayout::None, ImageLayout::General, ImageAspect::Color, mComputeImage);
    
    mComputeDescriptor.UpdateImage(mComputeImage, ImageLayout::General, mDefaultSampler, 0);

    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);
}


void Renderer::DisplayToWindow(const RenderTarget& target) 
{
    vkDeviceWaitIdle(getDevice());
    uint32_t imageIndex = mSwapchain.GetNextImageIndex(mImageAcquiredSemaphore, {});
    VkSwapchainKHR swapchain[] = {mSwapchain.GetHandle()};

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

    mTransferToSwapchainCommandBuffer.QueueSubmit(getQueues().transfer, mImageAcquiredSemaphore, mTransferSemaphore);

    VkSemaphore waitSemaphoreHandles[] = {mTransferSemaphore.GetHandle()};
    VkPresentInfoKHR presentInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = waitSemaphoreHandles,
        .swapchainCount = 1,
        .pSwapchains = swapchain,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(getQueues().present, &presentInfo);
}

const RenderPass& Renderer::GetDeferredRenderPass() const { return mDeferredRenderPass; }

void Renderer::CreateDeferredRenderPass() 
{
    mDeferredRenderPass.AddAttachment(ImageFormat::RGBA8, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferredRenderPass.AddAttachment(ImageFormat::RGBA32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferredRenderPass.AddAttachment(ImageFormat::RGBA16, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mDeferredRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);

    mDeferredRenderPass.AddSubpass({0,1,2}, {}, 3, PipelineBindPoint::Graphic);

    mDeferredRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests);

    mDeferredRenderPass.Create();
}

void Renderer::CreateAttachments(const glm::uvec2& size) 
{
    mDeferredAttachments.CreateAttachment(mSwapchain.GetSize());
}

void Renderer::ResizeAttachments(const glm::uvec2& size) 
{
    vkDeviceWaitIdle(getDevice());
    mDeferredAttachments.ResizeAttachment(size);
    mDeferredFrameBuffer.Destroy();
    CreateDeferredFrameBuffer(size);


    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.albedo, ImageLayout::ShaderRead, mDefaultSampler, 0);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.position, ImageLayout::ShaderRead, mDefaultSampler, 1);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.normal, ImageLayout::ShaderRead, mDefaultSampler, 2);
    mDeferredAttachmentDescriptor.UpdateImage(mDeferredAttachments.depth, ImageLayout::ShaderRead, mDefaultSampler, 3);

}

void Renderer::DestroyAttachments() 
{
    mDeferredAttachments.DestroyAttachment();
}

void Renderer::CreateDeferredFrameBuffer(const glm::uvec2& size) 
{
    std::initializer_list<Image> attachments = 
    {   mDeferredAttachments.albedo,
        mDeferredAttachments.position,
        mDeferredAttachments.normal,
        mDeferredAttachments.depth,
    };

    mDeferredFrameBuffer.Create(size, attachments, mDeferredRenderPass);
}