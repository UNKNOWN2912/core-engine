#pragma once
#include <vulkan/vulkan.h>
#include "VulkanTypes.hpp"
#include <Core/Window.hpp>

class VulkanHelper
{
public:
	static VkInstance CreateInstance();
	static VkPhysicalDevice GetPhysicalDevice(VkInstance instance, VkPhysicalDeviceType deviceType);
	static VkSurfaceKHR CreateSurface(VkInstance instance, const Window& window);
	static QueueIndices GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
	static Queues GetQueues(VkDevice device, const QueueIndices& queueIndices);
	static VkDevice CreateDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const QueueIndices& queueIndices);
	static VkCommandPool CreateCommandPool(VkDevice device);
	static VkSemaphore CreateSemaphore(VkDevice device);
	static VkFence CreateFence(VkDevice device);
	static VkCommandBuffer AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool);
	static VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect);
	static VkFramebuffer CreateFrameBuffer(VkDevice device, VkRenderPass renderPass, uint32_t width, uint32_t height, const std::vector<VkImageView> attachments);
	static VkShaderModule CreateShaderModuleFromFile(VkDevice device, std::string_view filename);
	static VkPipelineLayout CreatePipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushConstants);
	static VkBuffer CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);
	static VkMemoryRequirements GetBufferMemoryRequirements(VkDevice device, VkBuffer buffer);
	static VkMemoryRequirements GetImageMemoryRequirements(VkDevice device, VkImage image);
	static uint32_t GetMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties);
	static VkDeviceMemory AllocateDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex);
	static VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
	static VkDescriptorPool CreateDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets);
	static VkDescriptorSet AllocateDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool);
	static VkImage CreateImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, const glm::uvec2& size);
	static VkSampler CreateSampler(VkDevice device, VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode);

	template<typename T>
	static T& GetIdData(uint64_t id)
	{
		return *((T*)(id));
	}
};