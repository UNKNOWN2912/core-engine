#include "ImageView.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

void ImageView::CreateImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    VkImageViewCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image.handle,
            .viewType = GetVulkanViewType(type),
            .format = GetVulkanImageFormat(image.format),
            .components =
                {
                    .r = GetVulkanComponentSwizzle(swizzle.r),
                    .g = GetVulkanComponentSwizzle(swizzle.g),
                    .b = GetVulkanComponentSwizzle(swizzle.b),
                    .a = GetVulkanComponentSwizzle(swizzle.a),
                },
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspect),
                    .baseMipLevel = baseMipmapLevel,
                    .levelCount = mipmapCount,
                    .baseArrayLayer = baseLayer,
                    .layerCount = layerCount,
                },
        };

    vkCreateImageView(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}
void ImageView::CreateImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    VkImageViewCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image.GetHandle(),
            .viewType = GetVulkanViewType(type),
            .format = GetVulkanImageFormat(image.GetFormat()),
            .components =
                {
                    .r = GetVulkanComponentSwizzle(swizzle.r),
                    .g = GetVulkanComponentSwizzle(swizzle.g),
                    .b = GetVulkanComponentSwizzle(swizzle.b),
                    .a = GetVulkanComponentSwizzle(swizzle.a),
                },
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspect),
                    .baseMipLevel = baseMipmapLevel,
                    .levelCount = mipmapCount,
                    .baseArrayLayer = baseLayer,
                    .layerCount = layerCount,
                },
        };

    vkCreateImageView(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void ImageView::DestroyImageView()
{
    vkDestroyImageView(GraphicsContext::GetDevice(), mHandle, nullptr);
}

VkImageView ImageView::GetHandle() const
{
    return mHandle;
}
ViewType ImageView::GetViewType() const
{
    return mViewType;
}
ImageAspect ImageView::GetAspect() const
{
    return mAspect;
}
uint32_t ImageView::GetBaseLayer() const
{
    return mBaseLayer;
}
uint32_t ImageView::GetLayerCount() const
{
    return mLayerCount;
}
uint32_t ImageView::GetBaseMipmapLevel() const
{
    return mBaseMipmapLevel;
}
uint32_t ImageView::GetMipmapLevelCount() const
{
    return mMipmapLevelCount;
}
