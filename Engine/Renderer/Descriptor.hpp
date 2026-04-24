#pragma once
#include "Renderer/Types.hpp"
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "Utility.hpp"


class Descriptor
{
	public:
		void AddDescriptor(DescriptorType type, ShaderStage shaderStage);
		void Create();

		void UpdateBuffer(const Buffer& buffer, uint32_t binding);
		void UpdateImage(const Image& image, ImageLayout layout, VkSampler sampler, uint32_t binding);
		
		VkDescriptorSet GetDescriptorSet() const { return mSet; }
		VkDescriptorSetLayout GetDescriptorSetLayout() const { return mSetLayout; }
		VkDescriptorPool GetDescriptorPool() const { return mDescriptorPool; }
	private:
		void CreateDescriptorSetLayout();
		void CreateDescriptorPool();
		void AllocateDescriptorSet();

		std::unordered_map<VkDescriptorType, uint32_t> mDescriptorTypeCount;
		std::vector<VkDescriptorSetLayoutBinding> mDescriptorBinding;

		VkDescriptorSetLayout mSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSet mSet = VK_NULL_HANDLE;
};
