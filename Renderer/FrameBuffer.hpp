#pragma once
#include "Renderer/ImageView.hpp"
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

class RenderPass;

class FrameBuffer
{
public:
    VkFramebuffer GetHandle() const;

    FrameBuffer() = default;
    ~FrameBuffer();

    FrameBuffer(FrameBuffer &&frameBuffer) noexcept;
    FrameBuffer &operator=(FrameBuffer &&frameBuffer) noexcept;

    FrameBuffer(const FrameBuffer &frameBuffer) = delete;
    FrameBuffer &operator=(const FrameBuffer &) = delete;

    FrameBuffer(const glm::uvec2 &size, const std::vector<std::reference_wrapper<const ImageView>> &attachments, const RenderPass &renderPass, uint32_t layers = 1)
    {
        std::vector<VkImageView> views;
        for (const ImageView &view : attachments)
        {
            views.push_back(view.GetHandle());
        }
        Create(size, views, renderPass, layers);
    }
    FrameBuffer(const glm::uvec2 &size, const std::vector<VkImageView> &attachments, const RenderPass &renderPass, uint32_t layers = 1)
    {
        Create(size, attachments, renderPass, layers);
    }

private:
    void Create(const glm::uvec2 &size, const std::vector<VkImageView> &attachments, const RenderPass &renderPass, uint32_t layers = 1);
    void Destroy();

    VkFramebuffer mHandle = VK_NULL_HANDLE;
    glm::uvec2 mSize = {};
};
