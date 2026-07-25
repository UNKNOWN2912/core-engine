#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Image.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Types.hpp"
#include "Utility.hpp"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

class Descriptor
{
public:
    void AddDescriptor(DescriptorType type, ShaderStage shaderStage);
    void AddBindlessDescriptor(DescriptorType type, ShaderStage shaderStage, uint32_t count);
    void CreateDescriptor();
    void DestroyDescriptor();

    void UpdateBuffer(const Buffer &buffer, uint32_t binding);
    void UpdateImage(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding);
    void UpdateImage(const Image &image, const Sampler &sampler, uint32_t binding);
    void UpdateImage(const ImageView &view, ImageLayout layout, const Sampler &sampler, uint32_t binding);
    void UpdateImageIndex(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding, uint32_t index);

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
    std::vector<VkDescriptorBindingFlags> mBindingFlags;
    std::vector<uint32_t> mBindingDescriptorCount;

    VkDescriptorSetLayout mSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSet mSet = VK_NULL_HANDLE;

    VkDescriptorSetLayoutCreateFlags mSetLayoutFlag = 0;
    VkDescriptorSetLayoutBindingFlagsCreateInfo mBindingCreateInfo = {};
    bool mExtentedInfoRequired = false;
};
