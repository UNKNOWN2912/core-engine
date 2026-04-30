#pragma once
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

class FrameBuffer
{
    public:
        void Create(const glm::uvec2& size, std::initializer_list<Image> attachments, const RenderPass& renderPass);
        void Destroy();

        VkFramebuffer GetFrameBuffer() const;
    private:
        VkFramebuffer mHandle = VK_NULL_HANDLE;
        glm::uvec2 size;
};