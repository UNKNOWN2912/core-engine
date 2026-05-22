#pragma once
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

class RenderPass;

class FrameBuffer
{
public:
    void Destroy();

    [[nodiscard]] VkFramebuffer GetHandle() const;

    FrameBuffer() = default;
    FrameBuffer(const FrameBuffer &framebuffer) = default;
    FrameBuffer(FrameBuffer &&framebuffer) noexcept : mHandle(framebuffer.mHandle), mSize(framebuffer.mSize)
    {
        framebuffer.mHandle = VK_NULL_HANDLE;
        framebuffer.mSize = {0, 0};
    }
    FrameBuffer &operator=(const FrameBuffer &) = default;
    FrameBuffer &operator=(FrameBuffer &&framebuffer) noexcept
    {
        mHandle = framebuffer.mHandle;
        mSize = framebuffer.mSize;
        framebuffer.mHandle = VK_NULL_HANDLE;
        framebuffer.mSize = {0, 0};

        return *this;
    }
    FrameBuffer(const glm::uvec2 &size, std::initializer_list<Image> attachments, const RenderPass &renderPass);
    ~FrameBuffer();

private:
    VkFramebuffer mHandle = VK_NULL_HANDLE;
    glm::uvec2 mSize = {};
};
