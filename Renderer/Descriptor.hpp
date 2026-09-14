#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Types.hpp"
#include "Utility.hpp"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

struct DescriptorLayout
{
    DescriptorType type = DescriptorType::None;
    ShaderStage shaderStage = ShaderStage::None;
    uint32_t count = 1;
};

class Descriptor
{
public:
    void AddDescriptor(DescriptorType type, ShaderStage shaderStage);
    void AddBindlessDescriptor(DescriptorType type, ShaderStage shaderStage, uint32_t count);
    void Create();
    void Destroy();

    void UpdateBuffer(const Buffer &buffer, uint32_t binding) const;
    void UpdateImage(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding) const;
    void UpdateImage(const Image &image, const Sampler &sampler, uint32_t binding) const;
    void UpdateImage(const ImageView &view, ImageLayout layout, const Sampler &sampler, uint32_t binding) const;
    void UpdateImageIndex(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding, uint32_t index) const;
    void UpdateImageIndex(const Image &image, ImageLayout layout, const Sampler &sampler, uint32_t binding, uint32_t index) const;

    VkDescriptorSet GetDescriptorSet() const;
    VkDescriptorSetLayout GetDescriptorSetLayout() const;
    VkDescriptorPool GetDescriptorPool() const;

    Descriptor() = default;
    Descriptor(std::initializer_list<DescriptorLayout> layouts);
    Descriptor(const Descriptor &descriptor) = delete;
    Descriptor(Descriptor &&descriptor) noexcept;
    Descriptor &operator=(const Descriptor &descriptor) = delete;
    Descriptor &operator=(Descriptor &&descriptor) noexcept;

private:
    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void AllocateDescriptorSet();

    void DestroyDescriptorSetLayout();
    void DestroyDescriptorPool();
    void FreeDescriptorSet();

    std::unordered_map<VkDescriptorType, uint32_t> mDescriptorTypeCount;
    std::vector<VkDescriptorSetLayoutBinding> mDescriptorBinding;
    std::vector<VkDescriptorBindingFlags> mBindingFlags;
    std::vector<uint32_t> mBindingDescriptorCount;

    VkDescriptorSetLayout mSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet mSet = VK_NULL_HANDLE;

    VkDescriptorSetLayoutCreateFlags mSetLayoutFlag = 0;
    VkDescriptorSetLayoutBindingFlagsCreateInfo mBindingCreateInfo = {};
    bool mExtentedInfoRequired = false;
};
