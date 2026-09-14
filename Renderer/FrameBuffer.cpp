#include "FrameBuffer.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/RenderPass.hpp"

void FrameBuffer::Create(const glm::uvec2 &size, const std::vector<VkImageView> &attachments, const RenderPass &renderPass, uint32_t layers)
{
    VkFramebufferCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
            .renderPass = renderPass.GetHandle(),
            .attachmentCount = (uint32_t)attachments.size(),
            .pAttachments = attachments.data(),
            .width = size.x,
            .height = size.y,
            .layers = layers,
        };

    vkCreateFramebuffer(GraphicsContext::GetCurrentContext().GetDevice(), &createInfo, nullptr, &mHandle);
}

void FrameBuffer::Destroy()
{
    if (mHandle == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyFramebuffer(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, nullptr);
    mHandle = VK_NULL_HANDLE;
    mSize = {};
}
VkFramebuffer FrameBuffer::GetHandle() const
{
    return mHandle;
}
FrameBuffer::~FrameBuffer()
{
    Destroy();
}

FrameBuffer::FrameBuffer(FrameBuffer &&frameBuffer) noexcept
{
    mHandle = frameBuffer.mHandle;
    mSize = frameBuffer.mSize;
    frameBuffer.mHandle = VK_NULL_HANDLE;
}

FrameBuffer &FrameBuffer::operator=(FrameBuffer &&frameBuffer) noexcept
{
    Destroy();
    mHandle = frameBuffer.mHandle;
    mSize = frameBuffer.mSize;
    frameBuffer.mHandle = VK_NULL_HANDLE;
    return *this;
}
