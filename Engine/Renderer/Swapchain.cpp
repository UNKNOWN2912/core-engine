#include "Swapchain.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"

void Swapchain::Create(const glm::uvec2& size, PresentMode presentMode) 
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getPhysicalDevice(), getSurface(), &capabilities);
    mSize = {size.x, size.y};

    if(mSize.x > capabilities.maxImageExtent.width || mSize.y > capabilities.maxImageExtent.height)
    {
        mSize = {800, 600};
    }

    VkFormat format = VK_FORMAT_B8G8R8A8_SRGB;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    uint32_t imageCount = capabilities.minImageCount + 1 <= capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = getSurface(),
        .minImageCount = imageCount,
        .imageFormat = format,
        .imageColorSpace = colorSpace,
        .imageExtent = {size.x, size.y},
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = GetVulkanPresentMode(presentMode),
        .clipped = VK_TRUE,
    };

    vkCreateSwapchainKHR(getDevice(), &createInfo, nullptr, &mHandle);

    std::vector<VkImage> images;
    std::vector<VkImageView> views;

    vkGetSwapchainImagesKHR(getDevice(), mHandle, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(getDevice(), mHandle, &imageCount, images.data());


    for (VkImage image : images)
    {
        VkImageView view = CreateImageView(image, ImageFormat::BGRA8, ImageAspect::Color);
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
}

void Swapchain::Destroy() 
{
    vkDestroySwapchainKHR(getDevice(), mHandle, nullptr);
    for (Image& image : mImages) 
    {
        vkDestroyImageView(getDevice(), image.view, nullptr);
    }
    mImages.clear();
    mSize = {};
}
