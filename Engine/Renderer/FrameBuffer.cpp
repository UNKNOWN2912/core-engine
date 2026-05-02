#include "FrameBuffer.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/RenderPass.hpp"

void FrameBuffer::Create(const glm::uvec2& size, std::initializer_list<Image> attachments, const RenderPass& renderPass) 
{
    std::vector<VkImageView> attachmentViews;
    for (const Image& image : attachments)
    {
        attachmentViews.push_back(image.view);
    }

    VkFramebufferCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = renderPass.GetHandle(),
        .attachmentCount = (uint32_t)attachmentViews.size(),
        .pAttachments = attachmentViews.data(),
        .width = size.x,
        .height = size.y,
        .layers = 1,
    };

    vkCreateFramebuffer(getDevice(), &createInfo, nullptr, &mHandle);
}

void FrameBuffer::Destroy() 
{
    vkDestroyFramebuffer(getDevice(), mHandle, nullptr);
}

VkFramebuffer FrameBuffer::GetHandle() const { return mHandle; }

