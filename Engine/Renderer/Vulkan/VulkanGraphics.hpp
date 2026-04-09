#pragma once
#include <vulkan/vulkan.h>
#include <Renderer/Graphics.hpp>
#include "VulkanTypes.hpp"


struct GraphicsData
{
	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	QueueIndices queueIndices;
	Queues queues;
	VkCommandPool commandPool = VK_NULL_HANDLE;
};