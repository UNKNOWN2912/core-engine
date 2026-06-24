#pragma once
#include "Renderer/ImageView.hpp"
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

class RenderPass;

class FrameBuffer
{
public:
    void CreateFrameBuffer(const glm::uvec2 &size, std::initializer_list<ImageView> attachments, const RenderPass &renderPass, uint32_t layers = 1);
    void CreateFrameBuffer(std::initializer_list<ImageDeprecated> attachments, const RenderPass &renderPass, uint32_t layers = 1);
    void CreateFrameBuffer(std::initializer_list<Image> attachments, const RenderPass &renderPass, uint32_t layers = 1);

    void DestroyFrameBuffer();

    VkFramebuffer GetHandle() const;

private:
    VkFramebuffer mHandle = VK_NULL_HANDLE;
    glm::uvec2 mSize = {};
};
