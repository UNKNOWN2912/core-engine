#include "VulkanHelper.hpp"
#include <assert.h>
#include <vector>
#include <Core/Macro.hpp>
#include <GLFW/glfw3.h>

VkInstance VulkanHelper::CreateInstance()
{
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	uint32_t extensionCount;
	const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	uint32_t layerCount = 1;
	const char* layers[] = { "VK_LAYER_KHRONOS_validation" };

	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensions;

	createInfo.enabledLayerCount = layerCount;
	createInfo.ppEnabledLayerNames = layers;

	VkInstance instance;
	vkCreateInstance(&createInfo, nullptr, &instance);
	return instance;
}

VkPhysicalDevice VulkanHelper::GetPhysicalDevice(VkInstance instance, VkPhysicalDeviceType deviceType)
{
	assert(instance != VK_NULL_HANDLE);

	uint32_t count;
	vkEnumeratePhysicalDevices(instance, &count, nullptr);
	VkPhysicalDevice devices[8];
	vkEnumeratePhysicalDevices(instance, &count, devices);

	for (uint32_t i = 0; i < count; i++)
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(devices[i], &properties);

		if (properties.deviceType == deviceType)
		{
			return devices[i];
		}
	}

	ERROR("Failed to find suitable device");
	return VK_NULL_HANDLE;
}

VkSurfaceKHR VulkanHelper::CreateSurface(VkInstance instance, const Window& window)
{
	assert(instance != VK_NULL_HANDLE);

	VkSurfaceKHR surface;
	glfwCreateWindowSurface(instance, (GLFWwindow*)window.GetNativeWindow(), nullptr, &surface);
	return surface;
}

QueueIndices VulkanHelper::GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

	uint32_t count;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
	VkQueueFamilyProperties properties[8];
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, properties);

	QueueIndices queueIndices;

	for (uint32_t i = 0; i < count; i++)
	{
		if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueIndices.graphic == UINT32_MAX)
		{
			queueIndices.graphic = i;
		}
		if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueIndices.transfer == UINT32_MAX)
		{
			queueIndices.transfer = i;
		}
		if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && queueIndices.compute == UINT32_MAX)
		{
			queueIndices.compute = i;
		}

		VkBool32 supported = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
		if (supported && queueIndices.present == UINT32_MAX)
		{
			queueIndices.present = i;
		}
	}

	return queueIndices;
}

Queues VulkanHelper::GetQueues(VkDevice device, const QueueIndices& queueIndices)
{
	assert(device != VK_NULL_HANDLE);

	Queues queues;

	vkGetDeviceQueue(device, queueIndices.graphic, 0, &queues.graphic);
	vkGetDeviceQueue(device, queueIndices.present, 0, &queues.present);
	vkGetDeviceQueue(device, queueIndices.compute, 0, &queues.compute);
	vkGetDeviceQueue(device, queueIndices.transfer, 0, &queues.transfer);

	return queues;
}

VkDevice VulkanHelper::CreateDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const QueueIndices& queueIndices)
{
	assert(physicalDevice != VK_NULL_HANDLE);

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	const char* extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

	createInfo.enabledExtensionCount = 1;
	createInfo.ppEnabledExtensionNames = &extension;

	std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos;

	float priority = 1.f;

	VkDeviceQueueCreateInfo graphicQueueCreateInfo = {};
	graphicQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicQueueCreateInfo.queueCount = 1;
	graphicQueueCreateInfo.pQueuePriorities = &priority;
	graphicQueueCreateInfo.queueFamilyIndex = queueIndices.graphic;

	VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
	presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	presentQueueCreateInfo.queueCount = 1;
	presentQueueCreateInfo.pQueuePriorities = &priority;
	presentQueueCreateInfo.queueFamilyIndex = queueIndices.present;

	VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
	computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	computeQueueCreateInfo.queueCount = 1;
	computeQueueCreateInfo.pQueuePriorities = &priority;
	computeQueueCreateInfo.queueFamilyIndex = queueIndices.compute;

	VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
	transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	transferQueueCreateInfo.queueCount = 1;
	transferQueueCreateInfo.pQueuePriorities = &priority;
	transferQueueCreateInfo.queueFamilyIndex = queueIndices.transfer;

	queuesCreateInfos.push_back(graphicQueueCreateInfo);
	if (queueIndices.present != queueIndices.graphic && queueIndices.present != UINT32_MAX)
	{
		queuesCreateInfos.push_back(presentQueueCreateInfo);
	}

	if (queueIndices.compute != queueIndices.present && queueIndices.compute != queueIndices.graphic && queueIndices.present != UINT32_MAX)
	{
		queuesCreateInfos.push_back(computeQueueCreateInfo);
	}

	if (queueIndices.transfer != queueIndices.present && queueIndices.transfer != queueIndices.graphic && queueIndices.transfer != queueIndices.compute && queueIndices.present != UINT32_MAX)
	{
		queuesCreateInfos.push_back(transferQueueCreateInfo);
	}

	createInfo.pQueueCreateInfos = queuesCreateInfos.data();
	createInfo.queueCreateInfoCount = queuesCreateInfos.size();

	VkDevice device;
	vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	return device;
}

VkCommandPool VulkanHelper::CreateCommandPool(VkDevice device)
{
	assert(device != VK_NULL_HANDLE);

	VkCommandPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	VkCommandPool commandPool;
	vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
	return commandPool;
}

