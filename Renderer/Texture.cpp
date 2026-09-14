#include "Texture.hpp"
#include "Core/Macro.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Helper.hpp"
#include <Renderer/GraphicsContext.hpp>
#include <filesystem>
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>

uint32_t GetFormatChannelCount(ImageFormat format)
{
    switch (format)
    {
    case ImageFormat::None:
        return 0;
        break;
    case ImageFormat::R8:
        return 1;
        break;
    case ImageFormat::RG8:
        return 2;
        break;
    case ImageFormat::RGB8:
        return 3;
        break;
    case ImageFormat::RGBA8:
        return 4;
        break;
    case ImageFormat::R8U:
        return 1;
        break;
    case ImageFormat::RG8U:
        return 2;
        break;
    case ImageFormat::RGB8U:
        return 3;
        break;
    case ImageFormat::RGBA8U:
        return 4;
        break;
    case ImageFormat::R8UNORM:
        return 1;
        break;
    case ImageFormat::RG8UNORM:
        return 2;
        break;
    case ImageFormat::RGB8UNORM:
        return 3;
        break;
    case ImageFormat::RGBA8UNORM:
        return 4;
        break;
    case ImageFormat::R16:
        return 1;
        break;
    case ImageFormat::RG16:
        return 2;
        break;
    case ImageFormat::RGB16:
        return 3;
        break;
    case ImageFormat::RGBA16:
        return 4;
        break;
    case ImageFormat::R16U:
        return 1;
        break;
    case ImageFormat::RG16U:
        return 2;
        break;
    case ImageFormat::RGB16U:
        return 3;
        break;
    case ImageFormat::RGBA16U:
        return 4;
        break;
    case ImageFormat::R16UNORM:
        return 1;
        break;
    case ImageFormat::RG16UNORM:
        return 2;
        break;
    case ImageFormat::RGB16UNORM:
        return 3;
        break;
    case ImageFormat::RGBA16UNORM:
        return 4;
        break;
    case ImageFormat::R32:
        return 1;
        break;
    case ImageFormat::RG32:
        return 2;
        break;
    case ImageFormat::RGB32:
        return 3;
        break;
    case ImageFormat::RGBA32:
        return 4;
        break;
    case ImageFormat::R32U:
        return 1;
        break;
    case ImageFormat::RG32U:
        return 2;
        break;
    case ImageFormat::RGB32U:
        return 3;
        break;
    case ImageFormat::RGBA32U:
        return 4;
        break;
    case ImageFormat::R64:
        return 1;
        break;
    case ImageFormat::RG64:
        return 2;
        break;
    case ImageFormat::RGB64:
        return 3;
        break;
    case ImageFormat::RGBA64:
        return 4;
        break;
    case ImageFormat::R64U:
        return 1;
        break;
    case ImageFormat::RG64U:
        return 2;
        break;
    case ImageFormat::RGB64U:
        return 3;
        break;
    case ImageFormat::RGBA64U:
        return 4;
        break;
    case ImageFormat::D32:
        return 1;
        break;
    case ImageFormat::D24S8:
        return 2;
        break;
    case ImageFormat::BGRA8:
        return 4;
        break;
    case ImageFormat::BGRA8UNORM:
        return 4;
        break;
    default:
        break;
    }
}

void Texture::Create(void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter, Filter magFilter, AddressMode addressMode)
{
    CHROME_TRACE_FUNCTION();
    mImage = Image(size, format, ImageUsage::TransferDestination | ImageUsage::Sampler, ImageType::TwoDimensional, ImageAspect::Color, MemoryProperty::DeviceLocal, SampleCount::One, 1, 1);
    mImage.SetData(data, size);

    mSampler = Sampler(minFilter, magFilter, addressMode, addressMode, addressMode, false, CompareType::Always);
}

void Texture::Load(std::string_view filename, ImageFormat format, Filter minFilter, Filter magFilter, AddressMode addressMode)
{
    CHROME_TRACE_FUNCTION();

    if (!std::filesystem::exists(filename))
    {
        ERROR("File not found: {}", filename);
        assert(false);
    }

    glm::ivec2 size;
    stbi_uc *data = stbi_load(filename.data(), &size.x, &size.y, nullptr, 4);

    Create(data, size, format, minFilter, magFilter, addressMode);
    SetFilename(filename);
    SetName(filename.data());
    stbi_image_free(data);
}

const Image &Texture::GetImage() const
{
    return mImage;
}

Image &Texture::GetImage()
{
    return mImage;
}

const std::string &Texture::GetName() const
{
    return mName;
}
void Texture::SetName(const std::string &name)
{
    mName = name;
}
const Sampler &Texture::GetSampler() const
{
    return mSampler;
}

void Texture::SetFilename(std::string_view filename)
{
    mFileName = filename;
}

const std::string &Texture::GetFilename() const
{
    return mFileName;
}

void Texture::Destroy()
{
}
