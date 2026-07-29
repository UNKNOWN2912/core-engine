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
    void CreateImageView(const ImageDeprecated &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    void CreateImageView(const Image &image, ViewType type, ImageAspect aspect, uint32_t baseLayer = 0, uint32_t layerCount = 1, uint32_t baseMipmapLevel = 0, uint32_t mipmapCount = 1, const Swizzle &swizzle = {});
    void DestroyImageView();

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
};