#include "Texture.hpp"
#include "Core/Macro.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
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

    VkSamplerCreateInfo samplerCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .minLod = 1,
        .maxLod = 1,
    };

    vkCreateSampler(getDevice(), &samplerCreateInfo, nullptr, &mSampler);

    mIsValid = true;
}

void Texture::Load(std::string_view filename)
{
    CHROME_TRACE_FUNCTION();
    glm::ivec2 size;
    stbi_uc* data = stbi_load(filename.data(), &size.x, &size.y, nullptr, 4);

    Create(data, size, ImageFormat::RGBA8);
}


void Texture::UpdateDescriptorSet(VkDescriptorSet descriptorSet, uint32_t binding)
{
    CHROME_TRACE_FUNCTION();
    VkDescriptorImageInfo imageInfo = 
    {
        .sampler = mSampler,
        .imageView = mImage.view,
        .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    };

    VkWriteDescriptorSet write = 
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = descriptorSet,
        .dstBinding = binding,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
    };

    vkUpdateDescriptorSets(getDevice(), 1, &write, 0, nullptr);
}

void Texture::SetSampler(Filter minFilter, Filter magFilter, const std::array<AddressMode, 3>& addressMode) 
{
    if(mSampler != VK_NULL_HANDLE)
        vkDestroySampler(getDevice(), mSampler, nullptr);

    VkSamplerCreateInfo samplerCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = GetVulkanFilter(magFilter),
        .minFilter = GetVulkanFilter(minFilter),
        .addressModeU = GetVulkanAddressMode(addressMode[0]),
        .addressModeV = GetVulkanAddressMode(addressMode[1]),
        .addressModeW = GetVulkanAddressMode(addressMode[2]),
        .minLod = 1,
        .maxLod = 1,
    };

    vkCreateSampler(getDevice(), &samplerCreateInfo, nullptr, &mSampler);
}
