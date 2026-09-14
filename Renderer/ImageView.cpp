#include "ImageView.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan.h>

void ImageView::CreateImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    CreateImageView(image.handle, image.format, type, aspect, baseLayer, layerCount, baseMipmapLevel, mipmapCount, swizzle);
}
void ImageView::CreateImageView(VkImage image, ImageFormat format, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    VkImageViewCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = GetVulkanViewType(type),
            .format = GetVulkanImageFormat(format),
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

    vkCreateImageView(GraphicsContext::GetCurrentContext().GetDevice(), &createInfo, nullptr, &mHandle);

    SetValues(format, type, aspect, baseLayer, layerCount, baseMipmapLevel, mipmapCount, swizzle);
}
void ImageView::CreateImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    CreateImageView(image.GetHandle(), image.GetFormat(), type, aspect, baseLayer, layerCount, baseMipmapLevel, mipmapCount, swizzle);
}

void ImageView::DestroyImageView()
{
    vkDestroyImageView(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, nullptr);
}

void ImageView::SetValues(ImageFormat format, ViewType viewType, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapLevelCount, Swizzle swizzle)
{
    mFormat = format;
    mViewType = viewType;
    mAspect = aspect;
    mBaseLayer = baseLayer;
    mLayerCount = layerCount;
    mBaseMipmapLevel = baseMipmapLevel;
    mMipmapLevelCount = mipmapLevelCount;
    mSwizzle = swizzle;
}

ImageView::ImageView(ImageView &&view) noexcept
{
    SetValues(view.mFormat, view.mViewType, view.mAspect, view.mBaseLayer, view.mLayerCount, view.mBaseMipmapLevel, view.mMipmapLevelCount, view.mSwizzle);
    mHandle = view.mHandle;
    view.mHandle = VK_NULL_HANDLE;
}

ImageView &ImageView::operator=(ImageView &&view) noexcept
{
    DestroyImageView();
    SetValues(view.mFormat, view.mViewType, view.mAspect, view.mBaseLayer, view.mLayerCount, view.mBaseMipmapLevel, view.mMipmapLevelCount, view.mSwizzle);
    mHandle = view.mHandle;
    view.mHandle = VK_NULL_HANDLE;
    return *this;
}

ImageView::ImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    CreateImageView(image, type, aspect, baseLayer, layerCount, baseMipmapLevel, mipmapCount, swizzle);
}

ImageView::ImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle)
{
    CreateImageView(image, type, aspect, baseLayer, layerCount, baseMipmapLevel, mipmapCount, swizzle);
}

ImageView::~ImageView()
{
    DestroyImageView();
}

void ImageView::Copy(const ImageView &imageView, const Image &image)
{
    CreateImageView(image, imageView.GetViewType(), imageView.GetAspect(), imageView.GetBaseLayer(), imageView.GetLayerCount(), imageView.GetBaseMipmapLevel(), imageView.GetMipmapLevelCount());
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
