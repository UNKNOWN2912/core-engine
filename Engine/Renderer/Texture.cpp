#include "Texture.hpp"
#include "Core/Macro.hpp"
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>

void Texture::Create(void* data, const glm::uvec2& size, ImageFormat format)
{
    CHROME_TRACE_FUNCTION();
    mImage = CreateImage(size, format, ImageUsage::TransferDestination | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mStagingBuffer = CreateBuffer(mImage.memorySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    memcpy(mStagingBuffer.map, data, mImage.memorySize);

    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mImage);

    TransferImageData(mStagingBuffer, mImage, VK_IMAGE_ASPECT_COLOR_BIT);

    TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mImage);

    mIsValid = true;
}

void Texture::Load(std::string_view filename)
{
    CHROME_TRACE_FUNCTION();
    glm::ivec2 size;
    stbi_uc* data = stbi_load(filename.data(), &size.x, &size.y, nullptr, 4);

    Create(data, size, ImageFormat::RGBA8);
}
