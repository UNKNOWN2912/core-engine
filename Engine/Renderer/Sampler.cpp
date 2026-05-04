#include "Sampler.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <vulkan/vulkan_core.h>

void Sampler::SetFilter(Filter minification, Filter magnification)
{
    mMinFilter = GetVulkanFilter(minification);
    mMagFilter = GetVulkanFilter(magnification);
}
void Sampler::SetAddressMode(AddressMode u, AddressMode v, AddressMode w)
{
    mUAddressMode = GetVulkanAddressMode(u);
    mVAddressMode = GetVulkanAddressMode(v);
    mWAddressMode = GetVulkanAddressMode(w);
}
void Sampler::SetBorderColor(const glm::vec4& color)
{
    
}
void Sampler::Create()
{
    VkSamplerCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = mMagFilter,
        .minFilter = mMinFilter,
        .addressModeU = mUAddressMode,
        .addressModeV = mVAddressMode,
        .addressModeW = mWAddressMode,
        .minLod = 1,
        .maxLod = 1,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    };

    vkCreateSampler(getDevice(), &createInfo, nullptr, &mHandle);
}
void Sampler::Destroy() 
{
    vkDestroySampler(getDevice(), mHandle, nullptr);    
}
