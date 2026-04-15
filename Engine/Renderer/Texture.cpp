#include "Texture.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <memory.h>

#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>

void Texture::Create(void* data, VkFormat format, const glm::uvec2& size, uint32_t binding)
{
    mImage = CreateImage(size, format, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    mStagingBuffer = CreateBuffer(mImage.memorySize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    memcpy(mStagingBuffer.map, data, mImage.memorySize);

    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mImage);

    TransferImageData(mStagingBuffer, mImage, VK_IMAGE_ASPECT_COLOR_BIT);

    TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_ASPECT_COLOR_BIT, mImage);

    VkDescriptorSetLayoutBinding setLayoutBinding = 
    {
        .binding = binding,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkSamplerCreateInfo samplerCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_LINEAR,
        .minFilter = VK_FILTER_LINEAR,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .minLod = 1,
        .maxLod = 1,
    };

    vkCreateSampler(getDevice(), &samplerCreateInfo, nullptr, &mSampler);

    mIsValid = true;
}

void Texture::Load(std::string_view filename, uint32_t binding)
{
    int width, height;
    stbi_uc* data = stbi_load(filename.data(), &width, &height, nullptr, 4);

    Create(data, VK_FORMAT_R8G8B8A8_SRGB, {width, height}, binding);
}


void Texture::SetDataToDescriptorSet(VkDescriptorSet descriptorSet, uint32_t binding)
{
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
