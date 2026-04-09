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

};