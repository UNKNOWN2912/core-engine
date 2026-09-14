#pragma once
#include "Renderer/Types.hpp"
#include "Renderer/Utility.hpp"

class Image;

struct Swizzle
{
    ComponentSwizzle r = ComponentSwizzle::Identity;
    ComponentSwizzle g = ComponentSwizzle::Identity;
    ComponentSwizzle b = ComponentSwizzle::Identity;
    ComponentSwizzle a = ComponentSwizzle::Identity;
};

class ImageView
{
public:
    ImageView() = default;
    ImageView(const ImageView &view) = delete;
    ImageView(ImageView &&view) noexcept;
    ImageView &operator=(const ImageView &view) = delete;
    ImageView &operator=(ImageView &&view) noexcept;
    ImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    ImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    ~ImageView();

    void Copy(const ImageView &imageView, const Image &image);

    VkImageView GetHandle() const;
    ViewType GetViewType() const;
    ImageAspect GetAspect() const;
    uint32_t GetBaseLayer() const;
    uint32_t GetLayerCount() const;
    uint32_t GetBaseMipmapLevel() const;
    uint32_t GetMipmapLevelCount() const;

private:
    VkImageView mHandle = VK_NULL_HANDLE;

    ViewType mViewType = ViewType::TwoDimensional;
    ImageAspect mAspect = ImageAspect::Color;
    uint32_t mBaseLayer = 0;
    uint32_t mLayerCount = 1;
    uint32_t mBaseMipmapLevel = 0;
    uint32_t mMipmapLevelCount = 1;
    Swizzle mSwizzle;

    ImageFormat mFormat;

private:
    void CreateImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    void CreateImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    void CreateImageView(VkImage image, ImageFormat format, ViewType type, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapCount, const Swizzle &swizzle);
    void DestroyImageView();

    void SetValues(ImageFormat format, ViewType viewType, ImageAspect aspect, uint32_t baseLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapLevelCount, Swizzle swizzle);
};