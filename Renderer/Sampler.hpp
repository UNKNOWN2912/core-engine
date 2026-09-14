#pragma once
#include "Renderer/Types.hpp"
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>

struct SamplerConfig
{
    Filter minification = Filter::Nearest;
    Filter magnification = Filter::Nearest;
    AddressMode addressMode[3] = {AddressMode::Repeat, AddressMode::Repeat, AddressMode::Repeat};
    bool enableCompare = false;
    CompareType compareType;
};

class Sampler
{
public:
    VkSampler GetHandle() const;

    Sampler(Filter minFilter, Filter magFilter, AddressMode u, AddressMode v, AddressMode w, bool enableCompare, CompareType compareType);
    Sampler() = default;
    Sampler(const Sampler &) = delete;
    Sampler(Sampler &&sampler) noexcept;
    Sampler &operator=(Sampler &&sampler) noexcept;
    Sampler &operator=(const Sampler &) = delete;
    ~Sampler();

private:
    void SetFilter(Filter minification, Filter magnification);
    void SetAddressMode(AddressMode u, AddressMode v, AddressMode w);
    void SetBorderColor(const glm::vec4 &color);
    void EnableCompare(bool enable, CompareType compareType);
    void Create();
    void Destroy();

    VkSampler mHandle = VK_NULL_HANDLE;
    VkSamplerCreateInfo mCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .magFilter = VK_FILTER_NEAREST,
            .minFilter = VK_FILTER_NEAREST,
            .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            .compareEnable = VK_FALSE,
            .compareOp = VK_COMPARE_OP_ALWAYS,
            .minLod = 1,
            .maxLod = 1,
            .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
    };
};
