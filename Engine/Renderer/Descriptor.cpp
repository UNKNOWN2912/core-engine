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


void Descriptor::CreateDescriptorSetLayout()
{
	VkDescriptorSetLayoutCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = (uint32_t)mDescriptorBinding.size(),
        .pBindings = mDescriptorBinding.data(),
    };    

    vkCreateDescriptorSetLayout(getDevice(), &createInfo, nullptr, &mSetLayout);
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

	vkCreateDescriptorPool(getDevice(), &createInfo, nullptr, &mDescriptorPool);
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

	vkAllocateDescriptorSets(getDevice(), &allocateInfo, &mSet);
}


void Descriptor::UpdateBuffer(const Buffer& buffer, uint32_t binding)
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

	vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void Descriptor::UpdateImage(const Image& image, ImageLayout layout, VkSampler sampler, uint32_t binding)
{
	VkDescriptorImageInfo imageInfo = 
	{
		.sampler = sampler,
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

	vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
