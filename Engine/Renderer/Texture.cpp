#include "Texture.hpp"
#include "Core/Macro.hpp"
#include <filesystem>
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>

void Texture::Create(void* data, const glm::uvec2& size, ImageFormat format)
{
    CHROME_TRACE_FUNCTION();

    if(IsValid())
        Destroy();

    mImage = CreateImage(size, format, ImageUsage::TransferDestination | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mStagingBuffer = CreateBuffer(mImage.memorySize, BufferUsage::TransferSource, MemoryProperty::HostVisible | MemoryProperty::HostCoherent);
    memcpy(mStagingBuffer.map, data, mImage.memorySize);

    TransitionImageLayout(ImageLayout::None, ImageLayout::TransferDestination, ImageAspect::Color, mImage);

    TransferImageData(mStagingBuffer, mImage, ImageAspect::Color);

    TransitionImageLayout(ImageLayout::TransferDestination, ImageLayout::ShaderRead, ImageAspect::Color, mImage);

    mIsValid = true;
}

void Texture::Load(std::string_view filename)
{
    CHROME_TRACE_FUNCTION();

    if(!std::filesystem::exists(filename))
    {
        ERROR("File not found: {}", filename);
        assert(false);
    }

    glm::ivec2 size;
    stbi_uc* data = stbi_load(filename.data(), &size.x, &size.y, nullptr, 4);

    Create(data, size, ImageFormat::RGBA8);
}

void Texture::Destroy() 
{
    DestroyImage(mImage);
    DestroyBuffer(mStagingBuffer);

    mImage = {};
    mStagingBuffer = {};

    mIsValid = false;
}
