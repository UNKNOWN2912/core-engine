#include "Image.hpp"
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

size_t getSizePerPixel(ImageFormat format)
{
    switch (format) 
    {
        case ImageFormat::None:
            return 0;
        case ImageFormat::R8:
            return 1 * 1;
        case ImageFormat::RG8:
            return 2 * 1;
        case ImageFormat::RGB8:
            return 3 * 1;
        case ImageFormat::RGBA8:
            return 4 * 1;
        case ImageFormat::R16:
            return 1 * 2;
        case ImageFormat::RG16:
            return 2 * 2;
        case ImageFormat::RGB16:
            return 3 * 2;
        case ImageFormat::RGBA16:
            return 4 * 2;
        case ImageFormat::R32:
            return 1 * 4;
        case ImageFormat::RG32:
            return 2 * 4;
        case ImageFormat::RGB32:
            return 3 * 4;
        case ImageFormat::RGBA32:
            return 4 * 4;
        break;
    }
}


ImageFormat getFormatFromChannelCount(uint32_t channelCount)
{
    switch (channelCount) 
    {
        case 1:
            return ImageFormat::R8;
        case 2:
            return ImageFormat::RG8;
        case 3:
            return ImageFormat::RGB8;
        case 4:
            return ImageFormat::RGBA8;
        default:
            return ImageFormat::None;
    }
}
///////////////////////////////////
// HOST IMAGE /////////////////////
///////////////////////////////////

void HostImage::CopyData(byte* data, const glm::uvec2& size, ImageFormat format) 
{
    assert(data != nullptr);
    assert(size != glm::uvec2(0));
    assert(format != ImageFormat::None);

    size_t sizePerPixel = getSizePerPixel(format);

    mSize = size;
    mFormat = format;
    mData = new byte[size.x * size.y * sizePerPixel];

    memcpy(mData, data, size.x * size.y * sizePerPixel);
}

void HostImage::SetData(byte* data, const glm::uvec2& size, ImageFormat format) 
{
    assert(data != nullptr);
    assert(size != glm::uvec2(0));
    assert(format != ImageFormat::None);

    mSize = size;
    mData = data;
    mFormat = format;
}


void HostImage::LoadFromFile(std::string_view filename) 
{
    int width, height, channel;
    stbi_uc* data = stbi_load(filename.data(), &width, &height, &channel, 0);
    ImageFormat format = getFormatFromChannelCount(channel);
    glm::uvec2 size = glm::uvec2(width, height);

    CopyData(data, size, format);

    stbi_image_free(data);
}
