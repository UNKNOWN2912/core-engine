#pragma once
#include "Renderer/Types.hpp"
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

class Sampler
{
    public:
        void SetFilter(Filter minification, Filter magnification);
        void SetAddressMode(AddressMode u, AddressMode v, AddressMode w);
        void SetBorderColor(const glm::vec4& color);
        void Create();
        void Destroy();

        VkSampler GetHandle() const { return mHandle; }
    private:
        VkFilter mMinFilter = VK_FILTER_NEAREST, mMagFilter = VK_FILTER_NEAREST;
        VkSamplerAddressMode mUAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT, mVAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT, mWAddressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        VkSampler mHandle = VK_NULL_HANDLE;
};