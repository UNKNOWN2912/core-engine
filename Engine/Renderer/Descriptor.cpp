#include "Descriptor.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Types.hpp"
#include "Renderer/Utility.hpp"
#include <vector>
#include <vulkan/vulkan_core.h>

void Descriptor::AddDescriptor(DescriptorType type, ShaderStage shaderStage)
{
    mDescriptorTypeCount[GetVulkanDescriptorType(type)]++;

    VkDescriptorSetLayoutBinding binding =
        {
            .binding = (uint32_t)mDescriptorBinding.size(),
            .descriptorType = GetVulkanDescriptorType(type),
            .descriptorCount = 1,
            .stageFlags = GetVulkanShaderStage(shaderStage),
        };

    mDescriptorBinding.push_back(binding);
}
void Descriptor::Create()
{
    CreateDescriptorSetLayout();
    CreateDescriptorPool();
    AllocateDescriptorSet();
}

void Descriptor::Destroy()
{
    mDescriptorTypeCount.clear();
    mDescriptorBinding.clear();
    DestroyDescriptorPool();
    DestroyDescriptorSetLayout();
}

Descriptor::Descriptor(Descriptor &&descriptor) noexcept
    : mDescriptorTypeCount(std::move(descriptor.mDescriptorTypeCount)), mDescriptorBinding(std::move(descriptor.mDescriptorBinding)),
      mSetLayout(descriptor.mSetLayout), mDescriptorPool(descriptor.mDescriptorPool), mSet(descriptor.mSet)
{
}

Descriptor &Descriptor::operator=(Descriptor &&descriptor) noexcept
{
    vkDestroyDescriptorSetLayout(GraphicsContext::GetDevice(), mSetLayout, nullptr);
    vkDestroyDescriptorPool(GraphicsContext::GetDevice(), mDescriptorPool, nullptr);

    mDescriptorTypeCount = std::move(descriptor.mDescriptorTypeCount);
    mDescriptorBinding = std::move(descriptor.mDescriptorBinding);

    mDescriptorPool = descriptor.mDescriptorPool;
    mSet = descriptor.mSet;
    mSetLayout = descriptor.mSetLayout;

    descriptor.mDescriptorPool = VK_NULL_HANDLE;
    descriptor.mSet = VK_NULL_HANDLE;
    descriptor.mSetLayout = VK_NULL_HANDLE;

    return *this;
}

Descriptor::~Descriptor()
{
    Destroy();
}

void Descriptor::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = (uint32_t)mDescriptorBinding.size(),
            .pBindings = mDescriptorBinding.data(),
        };

    vkCreateDescriptorSetLayout(GraphicsContext::GetDevice(), &createInfo, nullptr, &mSetLayout);
}
void Descriptor::CreateDescriptorPool()
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    uint32_t maxSets = 0;
    for (auto [type, count] : mDescriptorTypeCount)
    {
        VkDescriptorPoolSize poolSize =
            {
                .type = type,
                .descriptorCount = count,
            };

        maxSets += count;
        poolSizes.push_back(poolSize);
    }

    VkDescriptorPoolCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = maxSets,
            .poolSizeCount = (uint32_t)poolSizes.size(),
            .pPoolSizes = poolSizes.data(),
        };

    vkCreateDescriptorPool(GraphicsContext::GetDevice(), &createInfo, nullptr, &mDescriptorPool);
}
void Descriptor::AllocateDescriptorSet()
{
    VkDescriptorSetAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = mDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &mSetLayout,
        };

    vkAllocateDescriptorSets(GraphicsContext::GetDevice(), &allocateInfo, &mSet);
}

void Descriptor::DestroyDescriptorSetLayout()
{
    if (mSetLayout == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyDescriptorSetLayout(GraphicsContext::GetDevice(), mSetLayout, nullptr);
    mSetLayout = VK_NULL_HANDLE;
}

void Descriptor::DestroyDescriptorPool()
{
    if (mDescriptorPool == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyDescriptorPool(GraphicsContext::GetDevice(), mDescriptorPool, nullptr);
    mDescriptorPool = VK_NULL_HANDLE;
}

void Descriptor::UpdateBuffer(const Buffer &buffer, uint32_t binding)
{
    VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = buffer.handle,
            .offset = 0,
            .range = buffer.size,
        };

    VkWriteDescriptorSet writeDescriptorSet =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mSet,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = mDescriptorBinding[binding].descriptorType,
            .pBufferInfo = &bufferInfo,
        };

    vkUpdateDescriptorSets(GraphicsContext::GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void Descriptor::UpdateImage(const Image &image, ImageLayout layout, const Sampler &sampler, uint32_t binding)
{
    VkDescriptorImageInfo imageInfo =
        {
            .sampler = sampler.GetHandle(),
            .imageView = image.view,
            .imageLayout = GetVulkanImageLayout(layout),
        };

    VkWriteDescriptorSet writeDescriptorSet =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mSet,
            .dstBinding = binding,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = mDescriptorBinding[binding].descriptorType,
            .pImageInfo = &imageInfo,
        };

    vkUpdateDescriptorSets(GraphicsContext::GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
VkDescriptorSet Descriptor::GetDescriptorSet() const
{
    return mSet;
}

VkDescriptorSetLayout Descriptor::GetDescriptorSetLayout() const
{
    return mSetLayout;
}
VkDescriptorPool Descriptor::GetDescriptorPool() const
{
    return mDescriptorPool;
}

Descriptor::operator VkDescriptorSet()
{
    return mSet;
}
