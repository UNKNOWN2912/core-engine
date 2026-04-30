#pragma once
#include "Renderer/Utility.hpp"
#include <vector>
#include <vulkan/vulkan.h>

class Swapchain
{
    public:
        VkSwapchainKHR GetHandle() const { return mHandle; }
        const std::vector<Image>& GetImages() const { return mImages; }
        const glm::uvec2& GetSize() const { return mSize; }
        uint32_t GetImageCount() const { return mImages.size(); }

        void Create(const glm::uvec2& size, PresentMode presentMode);
        void Destroy();
    private:
        VkSwapchainKHR mHandle = VK_NULL_HANDLE;
        std::vector<Image> mImages;
        glm::uvec2 mSize;
};