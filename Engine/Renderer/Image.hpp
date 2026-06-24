#pragma once
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/ImageView.hpp"
#include "Renderer/Types.hpp"
#include <vulkan/vulkan.h>

class Image
{
public:
    void CreateImage(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageType type = ImageType::TwoDimensional, ImageAspect aspect = ImageAspect::Color, MemoryProperty memoryProperty = MemoryProperty::DeviceLocal, SampleCount sampleCount = SampleCount::One, uint32_t layerCount = 1, uint32_t mipmapCount = 1, uint32_t depth = 1);
    void CreateColorAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount = SampleCount::One, uint32_t layerCount = 1, uint32_t mipmapCount = 1);
    void CreateDepthAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount = SampleCount::One, uint32_t layerCount = 1, uint32_t mipmapCount = 1);
    void CreateCubeMap(const glm::uvec2 &size, ImageFormat format, ImageUsage additionalUsage, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t mipmapCount);

    void DestroyImage();

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

private:
    ImageFormat mFormat = ImageFormat::None;
    ImageUsage mUsage = ImageUsage::None;
    ImageAspect mAspect = ImageAspect::None;
    SampleCount mSampleCount = SampleCount::None;
    glm::uvec2 mSize = {0, 0};
    ImageLayout mLayout = ImageLayout::None;

    uint32_t mMipmapCount = 0;
    uint32_t mLayerCount = 0;

    VkImage mHandle = VK_NULL_HANDLE;
    VkDeviceMemory mMemory = VK_NULL_HANDLE;
    VkDeviceSize mMemorySize = 0;

    ImageView mImageView;
};

uint32_t GetImageFormatComponentCount(ImageFormat format);
size_t GetImageFormatMemorySize(ImageFormat format);