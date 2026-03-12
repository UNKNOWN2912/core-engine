#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <vulkan/vulkan.h>

class VulkanSwapchain
{
  public:
    void Create(const glm::vec2& size, VkPhysicalDevice physicalDevice, VkDevice device,VkSurfaceKHR surface);
    void Destroy();
    uint32_t GetImageIndex(VkSemaphore semaphore);

    VkSwapchainKHR GetHandle() const
    {
        return mHandle;
    }
    const std::vector<VkImage> &GetImages() const
    {
        return mImages;
    }
    const std::vector<VkImageView> &GetImageViews() const
    {
        return mImageViews;
    }
    uint32_t GetImageCount() const
    {
        return mImageCount;
    }
    VkFormat GetFormat() const
    {
        return mFormat;
    }
    VkColorSpaceKHR GetColorSpace() const
    {
        return mColorSpace;
    }
    VkExtent2D GetExtent() const
    {
        return mExtent;
    }

  private:
    VkSwapchainKHR mHandle = VK_NULL_HANDLE;
    std::vector<VkImage> mImages;
    std::vector<VkImageView> mImageViews;
    uint32_t mImageCount = 0;
    VkFormat mFormat = VK_FORMAT_B8G8R8A8_SRGB;
    VkColorSpaceKHR mColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D mExtent = {800, 600};

    VkSurfaceKHR mSurface = VK_NULL_HANDLE;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice mDevice = VK_NULL_HANDLE;
};