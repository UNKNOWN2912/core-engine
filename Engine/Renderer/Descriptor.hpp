#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Types.hpp"
#include "Utility.hpp"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

class Descriptor
{
public:
    Descriptor() = default;
    Descriptor(const Descriptor &) = delete;
    Descriptor(Descriptor &&descriptor) noexcept;
    Descriptor &operator=(const Descriptor &) = delete;
    Descriptor &operator=(Descriptor &&descriptor) noexcept;
    ~Descriptor();

    void AddDescriptor(DescriptorType type, ShaderStage shaderStage);
    void Create();
    void Destroy();

    void UpdateBuffer(const Buffer &buffer, uint32_t binding);
    void UpdateImage(const Image &image, ImageLayout layout, const Sampler &sampler, uint32_t binding);

    VkDescriptorSet GetDescriptorSet() const;
    VkDescriptorSetLayout GetDescriptorSetLayout() const;
    VkDescriptorPool GetDescriptorPool() const;
    operator VkDescriptorSet();

private:
    void CreateDescriptorSetLayout();
    void CreateDescriptorPool();
    void AllocateDescriptorSet();

    void DestroyDescriptorSetLayout();
    void DestroyDescriptorPool();

    std::unordered_map<VkDescriptorType, uint32_t> mDescriptorTypeCount;
    std::vector<VkDescriptorSetLayoutBinding> mDescriptorBinding;

    VkDescriptorSetLayout mSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet mSet = VK_NULL_HANDLE;
};
