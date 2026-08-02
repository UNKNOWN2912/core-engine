#include "Swapchain.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"

const std::vector<ImageDeprecated> &Swapchain::GetImages() const
{
    return mImages;
}
const glm::uvec2 &Swapchain::GetSize() const
{
    return mSize;
}
uint32_t Swapchain::GetImageCount() const
{
    return mImages.size();
}

uint32_t Swapchain::GetNextImageIndex(const Semaphore &semaphore, const Fence &fence) const
{
    uint32_t imageIndex = UINT32_MAX;
    VkResult result = vkAcquireNextImageKHR(GraphicsContext::GetDevice(), mHandle, UINT64_MAX, semaphore.GetHandle(), fence.GetHandle(), &imageIndex);
    return imageIndex;
}

void Swapchain::CreateSwapchain(VkSurfaceKHR surface, ImageFormat format, ColorSpace colorSpace, PresentMode presentMode, ImageUsage usage)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsContext::GetPhysicalDevice(), surface, &capabilities);
    uint32_t imageCount = glm::min(capabilities.minImageCount + 1, capabilities.maxImageCount);
    if (capabilities.maxImageCount == 0)
    {
        imageCount = capabilities.minImageCount;
    }

    mSize.x = capabilities.maxImageExtent.width;
    mSize.y = capabilities.maxImageExtent.height;

    assert(SurfaceFormatSupported(surface, format, colorSpace));

    VkSwapchainCreateInfoKHR createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = surface,
            .minImageCount = imageCount,
            .imageFormat = GetVulkanImageFormat(format),
            .imageColorSpace = GetVulkanColorSpace(colorSpace),
            .imageExtent = {mSize.x, mSize.y},
            .imageArrayLayers = 1,
            .imageUsage = GetVulkanImageUsage(usage),
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = GetVulkanPresentMode(presentMode),
            .clipped = VK_TRUE,
        };

    vkCreateSwapchainKHR(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);

    std::vector<VkImage> images;
    std::vector<VkImageView> views;

    vkGetSwapchainImagesKHR(GraphicsContext::GetDevice(), mHandle, &imageCount, nullptr);
    images.resize(imageCount);
    views.resize(imageCount);
    vkGetSwapchainImagesKHR(GraphicsContext::GetDevice(), mHandle, &imageCount, images.data());

    for (uint32_t i = 0; i < imageCount; i++)
    {
        views[i] = CreateImageView(images[i], format, ImageAspect::Color);
    }

    for (size_t i = 0; i < images.size(); i++)
    {
        ImageDeprecated image;
        image.handle = images[i];
        image.view = views[i];
        image.size = mSize;
        mImages.push_back(image);
    }

    mFormat = format;
}

void Swapchain::DestroySwapchain()
{
    for (ImageDeprecated &image : mImages)
    {
        vkDestroyImageView(GraphicsContext::GetDevice(), image.view, nullptr);
    }
    vkDestroySwapchainKHR(GraphicsContext::GetDevice(), mHandle, nullptr);
    *this = Swapchain();
}
ImageFormat Swapchain::GetFormat() const
{
    return mFormat;
}
VkSwapchainKHR Swapchain::GetHandle() const
{
    return mHandle;
};

bool Swapchain::SurfaceFormatSupported(VkSurfaceKHR surface, ImageFormat format, ColorSpace colorSpace)
{
    uint32_t count = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsContext::GetPhysicalDevice(), surface, &count, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(GraphicsContext::GetPhysicalDevice(), surface, &count, formats.data());

    for (auto &[supportedFormat, supportedColorSpace] : formats)
    {
        if (format == GetNativeImageFormat(supportedFormat) && colorSpace == GetNativeColorSpace(supportedColorSpace))
        {
            return true;
        }
    }

    return false;
}
