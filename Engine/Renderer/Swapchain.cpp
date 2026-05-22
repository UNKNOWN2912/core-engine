#include "Swapchain.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"

uint32_t Swapchain::GetNextImageIndex(const Semaphore &semaphore, const Fence &fence) const
{
    uint32_t imageIndex;
    vkAcquireNextImageKHR(GraphicsContext::GetDevice(), mHandle, UINT64_MAX, semaphore.GetHandle(), fence.GetHandle(), &imageIndex);
    return imageIndex;
}

void Swapchain::CreateSwapchain(const glm::uvec2 &size, PresentMode presentMode)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GraphicsContext::GetPhysicalDevice(), GraphicsContext::GetSurface(), &capabilities);
    mSize = {size.x, size.y};

    if (mSize.x > capabilities.maxImageExtent.width || mSize.y > capabilities.maxImageExtent.height)
    {
        mSize = {800, 600};
    }

    ImageFormat format = ImageFormat::BGRA8;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    uint32_t imageCount = capabilities.minImageCount + 1 <= capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
            .surface = GraphicsContext::GetSurface(),
            .minImageCount = imageCount,
            .imageFormat = GetVulkanImageFormat(format),
            .imageColorSpace = colorSpace,
            .imageExtent = {size.x, size.y},
            .imageArrayLayers = 1,
            .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
            .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .preTransform = capabilities.currentTransform,
            .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            .presentMode = GetVulkanPresentMode(presentMode),
            .clipped = VK_TRUE,
        };

    vkCreateSwapchainKHR(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);

    std::vector<VkImage> images;
    std::vector<VkImageView> views;

    vkGetSwapchainImagesKHR(GraphicsContext::GetDevice(), mHandle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(GraphicsContext::GetDevice(), mHandle, &imageCount, images.data());

    for (VkImage image : images)
    {
        VkImageView view = CreateImageView(image, format, ImageAspect::Color);
        views.push_back(view);
    }

    for (int i = 0; i < images.size(); i++)
    {
        Image image;
        image.handle = images[i];
        image.view = views[i];
        image.size = mSize;
        mImages.push_back(image);
    }

    mFormat = format;
}

void Swapchain::Destroy()
{
    vkDestroySwapchainKHR(GraphicsContext::GetDevice(), mHandle, nullptr);
    for (Image &image : mImages)
    {
        vkDestroyImageView(GraphicsContext::GetDevice(), image.view, nullptr);
    }
    mImages.clear();
    mSize = {};
}
