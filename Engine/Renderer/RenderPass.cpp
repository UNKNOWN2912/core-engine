#include "RenderPass.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"

uint32_t RenderPass::AddAttachment(ImageFormat format, ImageLayout initialLayout, ImageLayout finalLayout, LoadOperation loadOp, StoreOperation storeOp, LoadOperation stencilLoadOp, StoreOperation stencilStoreOp, SampleCount sampleCount)
{
    VkAttachmentDescription2 description =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
            .format = GetVulkanImageFormat(format),
            .samples = GetVulkanSampleCount(sampleCount),
            .loadOp = GetVulkanLoadOperation(loadOp),
            .storeOp = GetVulkanStoreOperation(storeOp),
            .stencilLoadOp = GetVulkanLoadOperation(loadOp),
            .stencilStoreOp = GetVulkanStoreOperation(storeOp),
            .initialLayout = GetVulkanImageLayout(initialLayout),
            .finalLayout = GetVulkanImageLayout(finalLayout),
        };

    mAttachments.push_back(description);
    return mAttachments.size() - 1;
}

void RenderPass::AddSubpass(std::initializer_list<uint32_t> colorAttachments, std::initializer_list<uint32_t> inputAttachments, std::initializer_list<uint32_t> resolveAttachments, uint32_t depthAttachment, PipelineBindPoint bindPoint)
{
    VkAttachmentReference2 *colorAttachmentReference = new VkAttachmentReference2[colorAttachments.size()];
    VkAttachmentReference2 *inputAttachmentReference = new VkAttachmentReference2[inputAttachments.size()];
    VkAttachmentReference2 *resolveAttachmentReference = new VkAttachmentReference2[resolveAttachments.size()];
    VkAttachmentReference2 *depthAttachmentReference = new VkAttachmentReference2;

    uint32_t i = 0;
    for (uint32_t index : colorAttachments)
    {
        VkAttachmentReference2 reference =
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
        VkAttachmentReference2 reference =
            {
                .attachment = index,
                .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            };

        inputAttachmentReference[i] = reference;
        i++;
    }

    i = 0;
    for (uint32_t index : resolveAttachments)
    {
        VkAttachmentReference2 reference =
            {
                .attachment = index,
                .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            };

        resolveAttachmentReference[i] = reference;
        i++;
    }

    if (depthAttachment != UINT32_MAX)
    {
        VkAttachmentReference2 reference =
            {
                .attachment = depthAttachment,
                .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            };

        *depthAttachmentReference = reference;
    }

    VkSubpassDescription2 description =
        {
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
            .pipelineBindPoint = GetVulkanPipelineBindPoint(bindPoint),
            .inputAttachmentCount = (uint32_t)inputAttachments.size(),
            .pInputAttachments = inputAttachmentReference,
            .colorAttachmentCount = (uint32_t)colorAttachments.size(),
            .pColorAttachments = colorAttachmentReference,
        };

    if (resolveAttachments.size() != 0)
    {
        description.pResolveAttachments = resolveAttachmentReference;
    }

    if (depthAttachment != UINT32_MAX)
    {
        description.pDepthStencilAttachment = depthAttachmentReference;
    }

    mSubpasses.push_back(description);
}

void RenderPass::AddDependency(uint32_t sourceSubpass, uint32_t destinationSubpass, PipelineStage sourcePipelineStage, PipelineStage destinationPipelineStage)
{
    VkAccessFlags srcAccessFlags = 0;

    if ((sourcePipelineStage & PipelineStage::ColorAttachmentOutput) == PipelineStage::ColorAttachmentOutput)
    {
        srcAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::EarlyFragmentTests) == PipelineStage::EarlyFragmentTests)
    {
        srcAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::LateFragmentTests) == PipelineStage::LateFragmentTests)
    {
        srcAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::Transfer) == PipelineStage::Transfer)
    {
        srcAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    VkAccessFlags dstAccessFlags = 0;

    if ((sourcePipelineStage & PipelineStage::ColorAttachmentOutput) == PipelineStage::ColorAttachmentOutput)
    {
        dstAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::EarlyFragmentTests) == PipelineStage::EarlyFragmentTests)
    {
        dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::LateFragmentTests) == PipelineStage::LateFragmentTests)
    {
        dstAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    }
    if ((sourcePipelineStage & PipelineStage::Transfer) == PipelineStage::Transfer)
    {
        dstAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT;
    }

    VkSubpassDependency2 dependency =
        {
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
            .srcSubpass = sourceSubpass,
            .dstSubpass = destinationSubpass,
            .srcStageMask = GetVulkanPipelineStage(sourcePipelineStage),
            .dstStageMask = GetVulkanPipelineStage(destinationPipelineStage),
            .srcAccessMask = srcAccessFlags,
            .dstAccessMask = dstAccessFlags,
        };

    mDependencies.push_back(dependency);
}

void RenderPass::AddSubpass(const Subpass &subpass, PipelineBindPoint bindPoint)
{
    mSubpasses.push_back(subpass.GetSubpassDescription(bindPoint));
}

void RenderPass::CmdBeginRenderPass(const CommandBuffer &commandBuffer, const FrameBuffer &frameBuffer, const glm::uvec2 &size, std::initializer_list<VkClearValue> clearValues)
{
    VkRenderPassBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            .renderPass = mHandle,
            .framebuffer = frameBuffer.GetHandle(),
            .renderArea = {{0, 0}, {size.x, size.y}},
            .clearValueCount = (uint32_t)clearValues.size(),
            .pClearValues = clearValues.begin(),
        };

    vkCmdBeginRenderPass(commandBuffer.GetHandle(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void RenderPass::CmdEndRenderPass(const CommandBuffer &commandBuffer)
{
    vkCmdEndRenderPass(commandBuffer.GetHandle());
}

void RenderPass::CreateRenderPass()
{
    VkRenderPassCreateInfo2 createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
            .attachmentCount = (uint32_t)mAttachments.size(),
            .pAttachments = mAttachments.data(),
            .subpassCount = (uint32_t)mSubpasses.size(),
            .pSubpasses = mSubpasses.data(),
            .dependencyCount = (uint32_t)mDependencies.size(),
            .pDependencies = mDependencies.data(),
        };

    vkCreateRenderPass2(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);

    mSubpasses.clear();
    mAttachments.clear();
    mDependencies.clear();
}

void RenderPass::DestroyRenderPass()
{
    if (mHandle == VK_NULL_HANDLE)
    {
        return;
    }
    mAttachments.clear();
    mDependencies.clear();
    mSubpasses.clear();
    vkDestroyRenderPass(GraphicsContext::GetDevice(), mHandle, nullptr);
    mHandle = VK_NULL_HANDLE;
}

RenderPass::~RenderPass()
{
    DestroyRenderPass();
}
void Subpass::AddColorAttachment(uint32_t index)
{
    VkAttachmentReference2 reference =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        };
    mColorAttachmentReferences.push_back(reference);
}
void Subpass::AddInputAttachment(uint32_t index)
{
    VkAttachmentReference2 reference =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        };
    mInputAttachmentReferences.push_back(reference);
}
void Subpass::AddResolveAttachment(uint32_t index)
{
    VkAttachmentReference2 reference =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
        };
    mResolveAttachmentReferences.push_back(reference);
}
void Subpass::SetDepthAttachment(uint32_t index)
{
    VkAttachmentReference2 reference =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        };

    mDepthAttachmentReference = reference;
}
void Subpass::SetDepthResolveAttachment(uint32_t index)
{
    VkAttachmentReference2 reference =
        {
            .sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
            .attachment = index,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
        };

    mDepthResolveAttachmentReference = reference;

    mDepthResolveDescription =
        {
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE,
            .depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
            .stencilResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
            .pDepthStencilResolveAttachment = &mDepthResolveAttachmentReference,
        };
}

VkSubpassDescription2 Subpass::GetSubpassDescription(PipelineBindPoint bindPoint) const
{
    VkSubpassDescription2 description =
        {
            .sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
            .pipelineBindPoint = GetVulkanPipelineBindPoint(bindPoint),
            .inputAttachmentCount = (uint32_t)mInputAttachmentReferences.size(),
            .pInputAttachments = mInputAttachmentReferences.data(),
            .colorAttachmentCount = (uint32_t)mColorAttachmentReferences.size(),
            .pColorAttachments = mColorAttachmentReferences.data(),
            .pResolveAttachments = mResolveAttachmentReferences.data(),
        };

    if (mDepthAttachmentReference.sType == VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2)
    {
        description.pDepthStencilAttachment = &mDepthAttachmentReference;
    }

    if (mDepthResolveAttachmentReference.sType == VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2)
    {
        description.pNext = &mDepthResolveDescription;
    }

    return description;
}
