#include "VulkanSwapchain.hpp"


void VulkanSwapchain::Create(const glm::vec2& size, VkPhysicalDevice physicalDevice, VkDevice device,VkSurfaceKHR surface)
{
    assert(mHandle == VK_NULL_HANDLE);

    mPhysicalDevice = physicalDevice;
    mDevice = device;
    mSurface = surface;


    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mPhysicalDevice, mSurface, &capabilities);
    mExtent = capabilities.currentExtent;

    mExtent = {uint32_t(size.x), uint32_t(size.y)};

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, mSurface, &formatCount, formats.data());

    for (VkSurfaceFormatKHR format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            mFormat = VK_FORMAT_B8G8R8A8_SRGB;
            mColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }

    mImageCount = capabilities.minImageCount + 1 <= capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = {VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
    createInfo.clipped = VK_TRUE;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageColorSpace = mColorSpace;
    createInfo.imageFormat = mFormat;
    createInfo.imageExtent = mExtent;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.minImageCount = mImageCount;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.surface = mSurface;

    if (createInfo.imageExtent.width == 0 || createInfo.imageExtent.height == 0)
    {
        createInfo.imageExtent = {800, 600};
    }

    mExtent = createInfo.imageExtent;

    vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mHandle);

    vkGetSwapchainImagesKHR(mDevice, mHandle, &mImageCount, nullptr);
    mImages.resize(mImageCount);
    vkGetSwapchainImagesKHR(mDevice, mHandle, &mImageCount, mImages.data());

    for (VkImage image : mImages)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = {VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO};
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.image = image;
        createInfo.format = mFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.subresourceRange.levelCount = 1;

        VkImageView imageView;
        vkCreateImageView(device, &createInfo, nullptr, &imageView);
        mImageViews.push_back(imageView);

    }
}

void VulkanSwapchain::Destroy()
{
    for (VkImageView view : mImageViews)
    {
        vkDestroyImageView(mDevice, view, nullptr);
    }

    mImageViews.clear();
    mImageCount = 0;
    mImages.clear();
    vkDestroySwapchainKHR(mDevice, mHandle, nullptr);
}

uint32_t VulkanSwapchain::GetImageIndex(VkSemaphore semaphore)
{
    uint32_t index;
    vkAcquireNextImageKHR(mDevice, mHandle, UINT64_MAX, semaphore, VK_NULL_HANDLE, &index);
    return index;
}
