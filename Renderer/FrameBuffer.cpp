#include "FrameBuffer.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/RenderPass.hpp"

void FrameBuffer::CreateFrameBuffer(const glm::uvec2 &size, std::initializer_list<ImageView> attachments, const RenderPass &renderPass, uint32_t layers)
{
    std::vector<VkImageView> attachmentViews;
    for (const ImageView &image : attachments)
    {
        attachmentViews.push_back(image.GetHandle());
    }

    VkFramebufferCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass.GetHandle(),
            .attachmentCount = (uint32_t)attachmentViews.size(),
            .pAttachments = attachmentViews.data(),
            .width = size.x,
            .height = size.y,
            .layers = layers,
        };

    vkCreateFramebuffer(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void FrameBuffer::CreateFrameBuffer(std::initializer_list<ImageDeprecated> attachments, const RenderPass &renderPass, uint32_t layers)
{
    std::vector<VkImageView> attachmentViews;
    for (const ImageDeprecated &image : attachments)
    {
        attachmentViews.push_back(image.view);
    }

    VkFramebufferCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass.GetHandle(),
            .attachmentCount = (uint32_t)attachmentViews.size(),
            .pAttachments = attachmentViews.data(),
            .width = attachments.begin()[0].size.x,
            .height = attachments.begin()[0].size.y,
            .layers = layers,
        };

    vkCreateFramebuffer(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void FrameBuffer::CreateFrameBuffer(std::initializer_list<Image> attachments, const RenderPass &renderPass, uint32_t layers)
{
    std::vector<VkImageView> attachmentViews;
    for (const Image &image : attachments)
    {
        attachmentViews.push_back(image.GetImageView().GetHandle());
    }

    VkFramebufferCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass.GetHandle(),
            .attachmentCount = (uint32_t)attachmentViews.size(),
            .pAttachments = attachmentViews.data(),
            .width = attachments.begin()[0].GetSize().x,
            .height = attachments.begin()[0].GetSize().y,
            .layers = layers,
        };

    vkCreateFramebuffer(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void FrameBuffer::DestroyFrameBuffer()
{
    if (mHandle == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyFramebuffer(GraphicsContext::GetDevice(), mHandle, nullptr);
    mHandle = VK_NULL_HANDLE;
    mSize = {};
}
VkFramebuffer FrameBuffer::GetHandle() const
{
    return mHandle;
}