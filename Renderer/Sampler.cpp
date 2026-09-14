
#include "Sampler.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include <vulkan/vulkan_core.h>

Sampler::~Sampler()
{
    Destroy();
}
VkSampler Sampler::GetHandle() const
{
    return mHandle;
}
Sampler::Sampler(Filter minFilter, Filter magFilter, AddressMode u, AddressMode v, AddressMode w, bool enableCompare, CompareType compareType)
{
    SetFilter(minFilter, magFilter);
    SetAddressMode(u, v, w);
    EnableCompare(enableCompare, compareType);
    Create();
}

Sampler &Sampler::operator=(Sampler &&sampler) noexcept
{
    Destroy();

    mHandle = sampler.mHandle;
    sampler.mHandle = VK_NULL_HANDLE;

    return *this;
}
Sampler::Sampler(Sampler &&sampler) noexcept
{
    mHandle = sampler.mHandle;
    sampler.mHandle = VK_NULL_HANDLE;
}

void Sampler::SetFilter(Filter minification, Filter magnification)
{
    mCreateInfo.minFilter = GetVulkanFilter(minification);
    mCreateInfo.magFilter = GetVulkanFilter(magnification);
}
void Sampler::SetAddressMode(AddressMode u, AddressMode v, AddressMode w)
{
    mCreateInfo.addressModeU = GetVulkanAddressMode(u);
    mCreateInfo.addressModeV = GetVulkanAddressMode(v);
    mCreateInfo.addressModeW = GetVulkanAddressMode(w);
}
void Sampler::SetBorderColor(const glm::vec4 &color)
{
}

void Sampler::Create()
{
    vkCreateSampler(GraphicsContext::GetCurrentContext().GetDevice(), &mCreateInfo, nullptr, &mHandle);
}
void Sampler::Destroy()
{
    if (mHandle == VK_NULL_HANDLE)
        return;
    vkDestroySampler(GraphicsContext::GetCurrentContext().GetDevice(), mHandle, nullptr);
}

void Sampler::EnableCompare(bool enable, CompareType compareType)
{
    mCreateInfo.compareEnable = enable;
    mCreateInfo.compareOp = GetVulkanCompareType(compareType);
}
