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
    mBindingFlags.push_back(0);
    mBindingDescriptorCount.push_back(1);
}

void Descriptor::AddBindlessDescriptor(DescriptorType type, ShaderStage shaderStage, uint32_t count)
{
    mDescriptorTypeCount[GetVulkanDescriptorType(type)] += count;

    VkDescriptorSetLayoutBinding binding =
        {
            .binding = (uint32_t)mDescriptorBinding.size(),
            .descriptorType = GetVulkanDescriptorType(type),
            .descriptorCount = count,
            .stageFlags = GetVulkanShaderStage(shaderStage),
        };

    mDescriptorBinding.push_back(binding);

    mSetLayoutFlag = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

    mBindingFlags.push_back(VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                            VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
                            VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT);

    mExtentedInfoRequired = true;
    mBindingDescriptorCount.push_back(count);
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
    FreeDescriptorSet();
    DestroyDescriptorSetLayout();
    DestroyDescriptorPool();
}

void Descriptor::CreateDescriptorSetLayout()
{
    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .bindingCount = (uint32_t)mBindingFlags.size(),
            .pBindingFlags = mBindingFlags.data(),
        };

    VkDescriptorSetLayoutCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .flags = mSetLayoutFlag,
            .bindingCount = (uint32_t)mDescriptorBinding.size(),
            .pBindings = mDescriptorBinding.data(),
        };

    if (mExtentedInfoRequired)
    {
        createInfo.pNext = &bindingCreateInfo;
    }

    vkCreateDescriptorSetLayout(GraphicsContext::GetCurrentContext().GetDevice(), &createInfo, nullptr, &mSetLayout);
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

    if (mExtentedInfoRequired)
    {
        createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    }

    vkCreateDescriptorPool(GraphicsContext::GetCurrentContext().GetDevice(), &createInfo, nullptr, &mDescriptorPool);
}
void Descriptor::AllocateDescriptorSet()
{
    VkDescriptorSetVariableDescriptorCountAllocateInfo setVariableDescriptorCountAllocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .descriptorSetCount = (uint32_t)mBindingDescriptorCount.size(),
            .pDescriptorCounts = mBindingDescriptorCount.data(),
        };

    VkDescriptorSetAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = mDescriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &mSetLayout,
        };

    if (mExtentedInfoRequired)
    {
        allocateInfo.pNext = &setVariableDescriptorCountAllocateInfo;
    }

    vkAllocateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), &allocateInfo, &mSet);
}

void Descriptor::DestroyDescriptorSetLayout()
{
    if (mSetLayout == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyDescriptorSetLayout(GraphicsContext::GetCurrentContext().GetDevice(), mSetLayout, nullptr);
    mSetLayout = VK_NULL_HANDLE;
}

void Descriptor::DestroyDescriptorPool()
{
    if (mDescriptorPool == VK_NULL_HANDLE)
    {
        return;
    }
    vkDestroyDescriptorPool(GraphicsContext::GetCurrentContext().GetDevice(), mDescriptorPool, nullptr);
    mDescriptorPool = VK_NULL_HANDLE;
}

void Descriptor::FreeDescriptorSet()
{
    // Freed when descriptor pool is destroyed

    // vkFreeDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), mDescriptorPool, 1, &mSet);
}

void Descriptor::UpdateBuffer(const Buffer &buffer, uint32_t binding) const
{
    VkDescriptorBufferInfo bufferInfo =
        {
            .buffer = buffer.handle,
            .offset = 0,
            .range = buffer.capacity,
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

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void Descriptor::UpdateImage(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding) const
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

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
void Descriptor::UpdateImage(const Image &image, const Sampler &sampler, uint32_t binding) const
{
    VkDescriptorImageInfo imageInfo =
        {
            .sampler = sampler.GetHandle(),
            .imageView = image.GetImageView().GetHandle(),
            .imageLayout = GetVulkanImageLayout(image.GetLayout()),
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

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
void Descriptor::UpdateImage(const ImageView &view, ImageLayout layout, const Sampler &sampler, uint32_t binding) const
{
    VkDescriptorImageInfo imageInfo =
        {
            .sampler = sampler.GetHandle(),
            .imageView = view.GetHandle(),
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

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void Descriptor::UpdateImageIndex(const ImageDeprecated &image, ImageLayout layout, const Sampler &sampler, uint32_t binding, uint32_t index) const
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
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = mDescriptorBinding[binding].descriptorType,
            .pImageInfo = &imageInfo,
        };

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void Descriptor::UpdateImageIndex(const Image &image, ImageLayout layout, const Sampler &sampler, uint32_t binding, uint32_t index) const
{
    VkDescriptorImageInfo imageInfo =
        {
            .sampler = sampler.GetHandle(),
            .imageView = image.GetImageView().GetHandle(),
            .imageLayout = GetVulkanImageLayout(layout),
        };

    VkWriteDescriptorSet writeDescriptorSet =
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = mSet,
            .dstBinding = binding,
            .dstArrayElement = index,
            .descriptorCount = 1,
            .descriptorType = mDescriptorBinding[binding].descriptorType,
            .pImageInfo = &imageInfo,
        };

    vkUpdateDescriptorSets(GraphicsContext::GetCurrentContext().GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
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

Descriptor::Descriptor(std::initializer_list<DescriptorLayout> layouts)
{
    for (const DescriptorLayout &layout : layouts)
    {
        if (layout.count == 1)
        {
            AddDescriptor(layout.type, layout.shaderStage);
        }
        else
        {
            AddBindlessDescriptor(layout.type, layout.shaderStage, layout.count);
        }
    }

    Create();
}

Descriptor::Descriptor(Descriptor &&descriptor) noexcept
{
    mDescriptorTypeCount = std::move(descriptor.mDescriptorTypeCount);
    mDescriptorBinding = std::move(descriptor.mDescriptorBinding);
    mBindingFlags = std::move(descriptor.mBindingFlags);
    mBindingDescriptorCount = std::move(descriptor.mBindingDescriptorCount);
    mSetLayoutFlag = descriptor.mSetLayoutFlag;
    mBindingCreateInfo = descriptor.mBindingCreateInfo;
    mExtentedInfoRequired = descriptor.mExtentedInfoRequired;

    mSetLayout = descriptor.mSetLayout;
    mDescriptorPool = descriptor.mDescriptorPool;
    mSet = descriptor.mSet;

    descriptor.mSet = VK_NULL_HANDLE;
    descriptor.mDescriptorPool = VK_NULL_HANDLE;
    descriptor.mSetLayout = VK_NULL_HANDLE;
}

Descriptor &Descriptor::operator=(Descriptor &&descriptor) noexcept
{
    Destroy();

    mDescriptorTypeCount = std::move(descriptor.mDescriptorTypeCount);
    mDescriptorBinding = std::move(descriptor.mDescriptorBinding);
    mBindingFlags = std::move(descriptor.mBindingFlags);
    mBindingDescriptorCount = std::move(descriptor.mBindingDescriptorCount);
    mSetLayoutFlag = descriptor.mSetLayoutFlag;
    mBindingCreateInfo = descriptor.mBindingCreateInfo;
    mExtentedInfoRequired = descriptor.mExtentedInfoRequired;

    mSetLayout = descriptor.mSetLayout;
    mDescriptorPool = descriptor.mDescriptorPool;
    mSet = descriptor.mSet;

    descriptor.mSet = VK_NULL_HANDLE;
    descriptor.mDescriptorPool = VK_NULL_HANDLE;
    descriptor.mSetLayout = VK_NULL_HANDLE;

    return *this;
}
