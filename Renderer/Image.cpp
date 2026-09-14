#include "Image.hpp"
#include "Core/Macro.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <cstring>

void Image::Create(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageType type, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount, uint32_t depth, bool cubeMap)
{
    if (size.x == 0 || size.y == 0)
    {
        return;
    }
    VkImageCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = GetVulkanImageType(type),
            .format = GetVulkanImageFormat(format),
            .extent =
                {
                    .width = size.x,
                    .height = size.y,
                    .depth = 1,
                },
            .mipLevels = mipmapCount,
            .arrayLayers = layerCount,
            .samples = GetVulkanSampleCount(sampleCount),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = GetVulkanImageUsage(usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

    vkCreateImage(GraphicsContext::GetCurrentContext().GetDevice(), &createInfo, nullptr, &mHandle);

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, &requirements);

    VkMemoryAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = requirements.size,
            .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, GetVulkanMemoryProperty(memoryProperty)),
        };

    vkAllocateMemory(GraphicsContext::GetCurrentContext().GetDevice(), &allocateInfo, nullptr, &mMemory);
    vkBindImageMemory(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, mMemory, 0);

    ViewType viewType = ViewType::TwoDimensional;
    switch (type)
    {
    case ImageType::None:
        viewType = ViewType::TwoDimensional;
        break;
    case ImageType::OneDimensional:
        viewType = (layerCount == 1) ? ViewType::OneDimensional : ViewType::OneDimensionalArray;
        break;
    case ImageType::TwoDimensional:
        viewType = (layerCount == 1) ? ViewType::TwoDimensional : ViewType::TwoDimensionalArray;
        break;
    case ImageType::ThreeDimensional:
        viewType = ViewType::ThreeDimensional;
        break;
    }

    mFormat = format;
    mUsage = usage;
    mAspect = aspect;
    mSampleCount = sampleCount;
    mSize = size;
    mLayerCount = layerCount;
    mMipmapCount = mipmapCount;
    mImageType = type;
    mDepth = depth;
    mIsCubemap = cubeMap;
    mMemoryProperty = memoryProperty;
    mMemorySize = requirements.size;

    if (cubeMap)
    {
        viewType = ViewType::Cube;
    }

    mImageView = ImageView(*this, viewType, aspect, 0, layerCount, 0, mipmapCount);
}

Image::Image(Image &&image) noexcept
{

    mFormat = image.mFormat;
    mUsage = image.mUsage;
    mAspect = image.mAspect;
    mSampleCount = image.mSampleCount;
    mSize = image.mSize;
    mLayout = image.mLayout;
    mImageType = image.mImageType;
    mMipmapCount = image.mMipmapCount;
    mLayerCount = image.mLayerCount;
    mDepth = image.mDepth;
    mImageType = image.mImageType;

    mHandle = image.mHandle;
    mMemory = image.mMemory;
    mMemorySize = image.mMemorySize;

    mImageView = std::move(image.mImageView);

    image.mMemory = VK_NULL_HANDLE;
    image.mHandle = VK_NULL_HANDLE;
}

Image &Image::operator=(Image &&image) noexcept
{
    Destroy();

    mFormat = image.mFormat;
    mUsage = image.mUsage;
    mAspect = image.mAspect;
    mSampleCount = image.mSampleCount;
    mSize = image.mSize;
    mLayout = image.mLayout;
    mImageType = image.mImageType;
    mMipmapCount = image.mMipmapCount;
    mLayerCount = image.mLayerCount;
    mDepth = image.mDepth;
    mImageType = image.mImageType;

    mHandle = image.mHandle;
    mMemory = image.mMemory;
    mMemorySize = image.mMemorySize;

    mImageView = std::move(image.mImageView);

    image.mMemory = VK_NULL_HANDLE;
    image.mHandle = VK_NULL_HANDLE;

    return *this;
}

Image::~Image()
{
    Destroy();
}

Image::Image(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageType type, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount, uint32_t depth, bool isCubeMap)
{
    Create(size, format, usage, type, aspect, memoryProperty, sampleCount, layerCount, mipmapCount, depth, isCubeMap);
}

void Image::Copy(const Image &image)
{
    Create(image.mSize, image.mFormat, image.mUsage, image.mImageType, image.mAspect, image.mMemoryProperty, image.mSampleCount, image.mLayerCount, image.mMipmapCount, image.mDepth, image.mIsCubemap);
}

Image Image::CreateColorAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount)
{
    return Image(size, ImageFormat::BGRA8, ImageUsage::ColorAttachment | additionalUsage, ImageType::TwoDimensional, ImageAspect::Color, MemoryProperty::DeviceLocal, sampleCount, layerCount, mipmapCount, 1);
}
Image Image::CreateDepthAttachment(const glm::uvec2 &size, ImageUsage additionalUsage, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount)
{
    return Image(size, ImageFormat::D32, ImageUsage::DepthStencil | additionalUsage, ImageType::TwoDimensional, ImageAspect::Depth, MemoryProperty::DeviceLocal, sampleCount, layerCount, mipmapCount, 1);
}
Image Image::CreateCubeMap(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t mipmapCount)
{
    return Image(size, format, usage, ImageType::TwoDimensional, aspect, memoryProperty, sampleCount, 6, mipmapCount, 1, true);
}

void Image::Destroy()
{
    vkDestroyImage(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, nullptr);
    vkFreeMemory(GraphicsContext::GetCurrentContext().GetDevice(), mMemory, nullptr);
}
void Image::TransitionLayout(ImageLayout newLayout)
{
    CommandBuffer commandBuffer;
    commandBuffer.BeginRecording();
    VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout = GetVulkanImageLayout(mLayout),
            .newLayout = GetVulkanImageLayout(newLayout),
            .image = mHandle,
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(mAspect),
                    .baseMipLevel = 0,
                    .levelCount = mMipmapCount,
                    .baseArrayLayer = 0,
                    .layerCount = mLayerCount,
                },
        };

    vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    commandBuffer.EndRecording();

    commandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().transfer);
    vkQueueWaitIdle(GraphicsContext::GetCurrentContext().GetQueues().transfer);

    mLayout = newLayout;
}
void Image::CmdTransitionLayout(const CommandBuffer &commandBuffer, ImageLayout newLayout)
{
    VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout = GetVulkanImageLayout(mLayout),
            .newLayout = GetVulkanImageLayout(newLayout),
            .image = mHandle,
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(mAspect),
                    .baseMipLevel = 0,
                    .levelCount = mMipmapCount,
                    .baseArrayLayer = 0,
                    .layerCount = mLayerCount,
                },
        };

    vkCmdPipelineBarrier(commandBuffer.GetHandle(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    mLayout = newLayout;
}

void Image::SetData(const void *data, const glm::uvec2 &size, const glm::uvec2 &offset, uint32_t layerIndex, uint32_t mipmapIndex)
{
    Buffer buffer = CreateBuffer(mMemorySize, BufferUsage::TransferSource, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
    memcpy(buffer.map, data, GetImageFormatMemorySize(mFormat) * size.x * size.y);

    CommandBuffer commandBuffer = CommandBuffer(GraphicsContext::GetCurrentContext().GetCommandPool());
    commandBuffer.BeginRecording();

    CmdTransitionLayout(commandBuffer, ImageLayout::TransferDestination);

    VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask = GetVulkanImageAspect(mAspect),
                    .mipLevel = mipmapIndex,
                    .baseArrayLayer = layerIndex,
                    .layerCount = 1,
                },
            .imageOffset = {int32_t(offset.x), int32_t(offset.y), 0},
            .imageExtent = {size.x, size.y, 1},
        };

    vkCmdCopyBufferToImage(commandBuffer.GetHandle(), buffer.handle, mHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    CmdTransitionLayout(commandBuffer, ImageLayout::ShaderRead);

    commandBuffer.EndRecording();
    commandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().transfer);
    vkQueueWaitIdle(GraphicsContext::GetCurrentContext().GetQueues().transfer);
}

VkImage Image::GetHandle() const
{
    return mHandle;
}
ImageFormat Image::GetFormat() const
{
    return mFormat;
}
const glm::uvec2 &Image::GetSize() const
{
    return mSize;
}
ImageUsage Image::GetUsage() const
{
    return mUsage;
}
SampleCount Image::GetSampleCount() const
{
    return mSampleCount;
}
ImageAspect Image::GetAspect() const
{
    return mAspect;
}
const ImageView &Image::GetImageView() const
{
    return mImageView;
}
ImageLayout Image::GetLayout() const
{
    return mLayout;
}

ImageType Image::GetImageType() const
{
    return mImageType;
}
size_t Image::GetMemorySize() const
{
    return mMemorySize;
}

uint32_t GetImageFormatComponentCount(ImageFormat format)
{
    std::pair<ImageFormat, uint32_t> formatComponentMap[] =
        {
            {ImageFormat::None, 0},
            {ImageFormat::R8, 1},
            {ImageFormat::RG8, 2},
            {ImageFormat::RGB8, 3},
            {ImageFormat::RGBA8, 4},
            {ImageFormat::R8U, 1},
            {ImageFormat::RG8U, 2},
            {ImageFormat::RGB8U, 3},
            {ImageFormat::RGBA8U, 4},
            {ImageFormat::R8UNORM, 1},
            {ImageFormat::RG8UNORM, 2},
            {ImageFormat::RGB8UNORM, 3},
            {ImageFormat::RGBA8UNORM, 4},
            {ImageFormat::R16, 1},
            {ImageFormat::RG16, 2},
            {ImageFormat::RGB16, 3},
            {ImageFormat::RGBA16, 4},
            {ImageFormat::R16U, 1},
            {ImageFormat::RG16U, 2},
            {ImageFormat::RGB16U, 3},
            {ImageFormat::RGBA16U, 4},
            {ImageFormat::R16UNORM, 1},
            {ImageFormat::RG16UNORM, 2},
            {ImageFormat::RGB16UNORM, 3},
            {ImageFormat::RGBA16UNORM, 4},
            {ImageFormat::R32, 1},
            {ImageFormat::RG32, 2},
            {ImageFormat::RGB32, 3},
            {ImageFormat::RGBA32, 4},
            {ImageFormat::R32U, 1},
            {ImageFormat::RG32U, 2},
            {ImageFormat::RGB32U, 3},
            {ImageFormat::RGBA32U, 4},
            {ImageFormat::R64, 1},
            {ImageFormat::RG64, 2},
            {ImageFormat::RGB64, 3},
            {ImageFormat::RGBA64, 4},
            {ImageFormat::R64U, 1},
            {ImageFormat::RG64U, 2},
            {ImageFormat::RGB64U, 3},
            {ImageFormat::RGBA64U, 4},
            {ImageFormat::D32, 1},
            {ImageFormat::D24S8, 2},
            {ImageFormat::BGRA8, 4},
            {ImageFormat::BGRA8UNORM, 4},
        };

    for (auto [f, count] : formatComponentMap)
    {
        if (f == format)
        {
            return count;
        }
    }

    ERROR("Invalid format");
    return 0;
}

size_t GetImageFormatMemorySize(ImageFormat format)
{
    std::pair<ImageFormat, size_t> formatComponentMap[] =
        {
            {ImageFormat::None, 0 * 0},
            {ImageFormat::R8, 1 * 1},
            {ImageFormat::RG8, 1 * 2},
            {ImageFormat::RGB8, 1 * 3},
            {ImageFormat::RGBA8, 1 * 4},
            {ImageFormat::R8U, 1 * 1},
            {ImageFormat::RG8U, 1 * 2},
            {ImageFormat::RGB8U, 1 * 3},
            {ImageFormat::RGBA8U, 1 * 4},
            {ImageFormat::R8UNORM, 1 * 1},
            {ImageFormat::RG8UNORM, 1 * 2},
            {ImageFormat::RGB8UNORM, 1 * 3},
            {ImageFormat::RGBA8UNORM, 1 * 4},
            {ImageFormat::R16, 2 * 1},
            {ImageFormat::RG16, 2 * 2},
            {ImageFormat::RGB16, 2 * 3},
            {ImageFormat::RGBA16, 2 * 4},
            {ImageFormat::R16U, 2 * 1},
            {ImageFormat::RG16U, 2 * 2},
            {ImageFormat::RGB16U, 2 * 3},
            {ImageFormat::RGBA16U, 2 * 4},
            {ImageFormat::R16UNORM, 2 * 1},
            {ImageFormat::RG16UNORM, 2 * 2},
            {ImageFormat::RGB16UNORM, 2 * 3},
            {ImageFormat::RGBA16UNORM, 2 * 4},
            {ImageFormat::R32, 4 * 1},
            {ImageFormat::RG32, 4 * 2},
            {ImageFormat::RGB32, 4 * 3},
            {ImageFormat::RGBA32, 4 * 4},
            {ImageFormat::R32U, 4 * 1},
            {ImageFormat::RG32U, 4 * 2},
            {ImageFormat::RGB32U, 4 * 3},
            {ImageFormat::RGBA32U, 4 * 4},
            {ImageFormat::R64, 8 * 1},
            {ImageFormat::RG64, 8 * 2},
            {ImageFormat::RGB64, 8 * 3},
            {ImageFormat::RGBA64, 8 * 4},
            {ImageFormat::R64U, 8 * 1},
            {ImageFormat::RG64U, 8 * 2},
            {ImageFormat::RGB64U, 8 * 3},
            {ImageFormat::RGBA64U, 8 * 4},
            {ImageFormat::D32, 4 * 1},
            {ImageFormat::D24S8, 4 * 2},
            {ImageFormat::BGRA8, 1 * 4},
            {ImageFormat::BGRA8UNORM, 1 * 4},
        };

    for (auto [f, count] : formatComponentMap)
    {
        if (f == format)
        {
            return count;
        }
    }

    ERROR("Invalid format");
    return 0;
}
