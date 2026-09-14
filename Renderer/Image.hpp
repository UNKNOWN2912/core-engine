#pragma once
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/ImageView.hpp"
#include "Renderer/Types.hpp"
#include <vulkan/vulkan.h>

class Image
{
public:
    Image() = default;

    Image(const Image &image) = delete;
    Image(Image &&image) noexcept;
    Image &operator=(const Image &image) = delete;
    Image &operator=(Image &&image) noexcept;
    ~Image();
    Image(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageType type = ImageType::TwoDimensional, ImageAspect aspect = ImageAspect::Color, MemoryProperty memoryProperty = MemoryProperty::DeviceLocal, SampleCount sampleCount = SampleCount::One, uint32_t layerCount = 1, uint32_t mipmapCount = 1, uint32_t depth = 1, bool isCubeMap = false);

    void Copy(const Image &image);

    static Image CreateColorAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount);
    static Image CreateDepthAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount);
    static Image CreateCubeMap(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t mipmapCount);

    void TransitionLayout(ImageLayout newLayout);
    void CmdTransitionLayout(const CommandBuffer &commandBuffer, ImageLayout newLayout);

    void SetData(const void *data, const glm::uvec2 &size, const glm::uvec2 &offset = {0, 0}, uint32_t layerIndex = 0, uint32_t mipmapIndex = 0);

    VkImage GetHandle() const;
    ImageFormat GetFormat() const;
    const glm::uvec2 &GetSize() const;
    ImageUsage GetUsage() const;
    SampleCount GetSampleCount() const;
    ImageAspect GetAspect() const;
    const ImageView &GetImageView() const;
    ImageLayout GetLayout() const;
    ImageType GetImageType() const;

    size_t GetMemorySize() const;

private:
    void Destroy();
    void Create(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageType type = ImageType::TwoDimensional, ImageAspect aspect = ImageAspect::Color, MemoryProperty memoryProperty = MemoryProperty::DeviceLocal, SampleCount sampleCount = SampleCount::One, uint32_t layerCount = 1, uint32_t mipmapCount = 1, uint32_t depth = 1, bool cubeMap = false);

    ImageFormat mFormat = ImageFormat::None;
    ImageUsage mUsage = ImageUsage::None;
    ImageAspect mAspect = ImageAspect::None;
    SampleCount mSampleCount = SampleCount::None;
    glm::uvec2 mSize = {0, 0};
    ImageLayout mLayout = ImageLayout::None;
    ImageView mImageView;
    ImageType mImageType;
    uint32_t mMipmapCount = 0;
    uint32_t mLayerCount = 0;
    MemoryProperty mMemoryProperty;
    uint32_t mDepth = 1;
    bool mIsCubemap = false;

    VkImage mHandle = VK_NULL_HANDLE;
    VkDeviceMemory mMemory = VK_NULL_HANDLE;
    VkDeviceSize mMemorySize = 0;
};

uint32_t GetImageFormatComponentCount(ImageFormat format);
size_t GetImageFormatMemorySize(ImageFormat format);