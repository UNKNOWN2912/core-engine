#pragma once
#include "Renderer/Synchronization.hpp"
#include "Renderer/Utility.hpp"
#include <vector>
#include <vulkan/vulkan.h>

class Swapchain
{
public:
    void CreateSwapchain(VkSurfaceKHR surface, ImageFormat format, ColorSpace colorSpace, PresentMode presentMode, ImageUsage usage = ImageUsage::ColorAttachment);
    void DestroySwapchain();

    uint32_t GetNextImageIndex(const Semaphore &semaphore, const Fence &fence) const;
    ImageFormat GetFormat() const;
    VkSwapchainKHR GetHandle() const;
    const std::vector<ImageDeprecated> &GetImages() const;
    const glm::uvec2 &GetSize() const;
    uint32_t GetImageCount() const;

private:
    VkSwapchainKHR mHandle = VK_NULL_HANDLE;
    std::vector<ImageDeprecated> mImages;
    glm::uvec2 mSize;
    ImageFormat mFormat;

private:
    bool SurfaceFormatSupported(VkSurfaceKHR surface, ImageFormat format, ColorSpace colorSpace);
};
