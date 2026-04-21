#include "RenderPass.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"

void RenderPass::AddAttachment(ImageFormat format, ImageLayout finalLayout, LoadOperation loadOp, StoreOperation storeOp, LoadOperation stencilLoadOp, StoreOperation stencilStoreOp, SampleCount sampleCount) 
{
    VkAttachmentDescription description = 
    {
        .format = GetVulkanImageFormat(format),
        .samples = GetVulkanSampleCount(sampleCount),
        .loadOp = GetVulkanLoadOperation(loadOp),
        .storeOp = GetVulkanStoreOperation(storeOp),
        .stencilLoadOp = GetVulkanLoadOperation(loadOp),
        .stencilStoreOp = GetVulkanStoreOperation(storeOp),
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = GetVulkanImageLayout(finalLayout),
    };

    mAttachments.push_back(description);
}

void RenderPass::AddSubpass(std::initializer_list<uint32_t> colorAttachments, std::initializer_list<uint32_t> inputAttachments, uint32_t depthAttachment) 
{
    VkAttachmentReference* colorAttachmentReference = new VkAttachmentReference[colorAttachments.size()];
    VkAttachmentReference* inputAttachmentReference = new VkAttachmentReference[inputAttachments.size()];
    VkAttachmentReference* depthAttachmentReference = new VkAttachmentReference;

    uint32_t i = 0;
    for (uint32_t index : colorAttachments) 
    {
        VkAttachmentReference reference = 
        {
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        colorAttachmentReference[i] = reference;
        i++;
    }

    i = 0;
    for (uint32_t index : inputAttachments) 
    {
        VkAttachmentReference reference = 
        {
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        inputAttachmentReference[i] = reference;
        i++;
    }

    if(depthAttachment != UINT32_MAX)
    {
        VkAttachmentReference reference = 
        {
            .attachment = depthAttachment,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };

        *depthAttachmentReference = reference;
    }

    VkSubpassDescription description = 
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = (uint32_t)inputAttachments.size(),
        .pInputAttachments = inputAttachmentReference,
        .colorAttachmentCount = (uint32_t)colorAttachments.size(),
        .pColorAttachments = colorAttachmentReference,
    };

    if(depthAttachment != UINT32_MAX)
    {
        description.pDepthStencilAttachment = depthAttachmentReference;
    }

    mSubpasses.push_back(description);
}

void RenderPass::AddDependency(uint32_t sourceSubpass, uint32_t destinationSubpass, PipelineStage sourcePipelineStage, PipelineStage destinationPipelineStage) 
{

    VkAccessFlags srcAccessFlags = 0;
    
    if((sourcePipelineStage & PipelineStage::ColorAttachmentOutput) == PipelineStage::ColorAttachmentOutput)
    {
        srcAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if((sourcePipelineStage & PipelineStage::EarlyFragmentTests) == PipelineStage::EarlyFragmentTests)
    {
        srcAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if((sourcePipelineStage & PipelineStage::LateFragmentTests) == PipelineStage::LateFragmentTests)
    {
        srcAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if((sourcePipelineStage & PipelineStage::Transfer) == PipelineStage::Transfer)
    {
        srcAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    VkAccessFlags dstAccessFlags = 0;
        
    if((sourcePipelineStage & PipelineStage::ColorAttachmentOutput) == PipelineStage::ColorAttachmentOutput)
    {
        dstAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    }
    if((sourcePipelineStage & PipelineStage::EarlyFragmentTests) == PipelineStage::EarlyFragmentTests)
    {
        dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if((sourcePipelineStage & PipelineStage::LateFragmentTests) == PipelineStage::LateFragmentTests)
    {
        dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if((sourcePipelineStage & PipelineStage::Transfer) == PipelineStage::Transfer)
    {
        dstAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    }

    VkSubpassDependency dependency = 
    {
        .srcSubpass = sourceSubpass,
        .dstSubpass = destinationSubpass,
        .srcStageMask = GetVulkanPipelineStage(sourcePipelineStage),
        .dstStageMask = GetVulkanPipelineStage(destinationPipelineStage),
        .srcAccessMask = srcAccessFlags,
        .dstAccessMask = dstAccessFlags,
    };

    mDependencies.push_back(dependency);
}

void RenderPass::CmdBeginRenderPass(const CommandBuffer& commandBuffer, VkFramebuffer framebuffer, const glm::uvec2& size, std::initializer_list<VkClearValue> clearValues) 
{
    VkRenderPassBeginInfo beginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = mHandle,
        .framebuffer = framebuffer,
        .renderArea = {{0,0}, {size.x, size.y}},
        .clearValueCount = (uint32_t)clearValues.size(),
        .pClearValues = clearValues.begin(),
    };

    vkCmdBeginRenderPass(commandBuffer.GetHandle(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::CmdEndRenderPass(const CommandBuffer& commandBuffer) 
{
    vkCmdEndRenderPass(commandBuffer.GetHandle());
}

void RenderPass::Create() 
{
    VkRenderPassCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = (uint32_t)mAttachments.size(),
        .pAttachments = mAttachments.data(),
        .subpassCount = (uint32_t)mSubpasses.size(),
        .pSubpasses = mSubpasses.data(),
        .dependencyCount = (uint32_t)mDependencies.size(),
        .pDependencies = mDependencies.data(),
    };

    vkCreateRenderPass(getDevice(), &createInfo, nullptr, &mHandle);    

    for (int i = 0; i < mSubpasses.size(); i++)
    {
        delete[] mSubpasses[i].pColorAttachments;
        delete[] mSubpasses[i].pInputAttachments;
        delete mSubpasses[i].pDepthStencilAttachment;
    }

    mSubpasses.clear();
    mAttachments.clear();
    mDependencies.clear();
}
void RenderPass::Destroy() 
{
    vkDestroyRenderPass(getDevice(), mHandle, nullptr);    
}
