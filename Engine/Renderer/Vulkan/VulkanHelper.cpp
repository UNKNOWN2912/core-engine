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

VkSemaphore VulkanHelper::CreateSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkSemaphore semaphore;
	vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
	return semaphore;
}

VkFence VulkanHelper::CreateFence(VkDevice device)
{
	VkFenceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkFence fence;
	vkCreateFence(device, &createInfo, nullptr, &fence);
	return fence;
}

VkCommandBuffer VulkanHelper::AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	return commandBuffer;
}

VkImageView VulkanHelper::CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect)
{
	VkImageViewCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	createInfo.format = format;
	createInfo.image = image;
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspect;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.layerCount = 1;
	createInfo.subresourceRange.levelCount = 1;

	VkImageView view;
	vkCreateImageView(device, &createInfo, nullptr, &view);
	return view;
}

VkFramebuffer VulkanHelper::CreateFrameBuffer(VkDevice device, VkRenderPass renderPass, uint32_t width, uint32_t height, const std::vector<VkImageView> attachments)
{
	VkFramebufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	createInfo.attachmentCount = attachments.size();
	createInfo.pAttachments = attachments.data();
	createInfo.width = width;
	createInfo.height = height;
	createInfo.layers = 1;
	createInfo.renderPass = renderPass;

	VkFramebuffer frameBuffer;
	vkCreateFramebuffer(device, &createInfo, nullptr, &frameBuffer);
	return frameBuffer;
}

VkShaderModule VulkanHelper::CreateShaderModuleFromFile(VkDevice device, std::string_view filename)
{
	FILE* fp = fopen(filename.data(), "rb");

	if (fp == NULL)
	{
		ERROR("Failed to find file {}", filename);
		return VK_NULL_HANDLE;
	}

	fseek(fp, 0L, SEEK_END);
	size_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	std::vector<char> code(size);

	fread(code.data(), 1, size, fp);
	fclose(fp);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = size;
	createInfo.pCode = (uint32_t*)code.data();

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	return shaderModule;
}

VkPipelineLayout VulkanHelper::CreatePipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushConstants)
{
	VkPipelineLayoutCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount = setLayouts.size();
	createInfo.pSetLayouts = setLayouts.data();
	createInfo.pushConstantRangeCount = pushConstants.size();
	createInfo.pPushConstantRanges = pushConstants.data();

	VkPipelineLayout pipelineLayout;
	vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout);
	return pipelineLayout;
}

VkBuffer VulkanHelper::CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage)
{
	VkBufferCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.usage = usage;
	createInfo.size = size;

	VkBuffer buffer;
	vkCreateBuffer(device, &createInfo, nullptr, &buffer);
	return buffer;
}

VkMemoryRequirements VulkanHelper::GetBufferMemoryRequirements(VkDevice device, VkBuffer buffer)
{
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(device, buffer, &requirements);
	return requirements;
}

VkMemoryRequirements VulkanHelper::GetImageMemoryRequirements(VkDevice device, VkImage image)
{
	VkMemoryRequirements requirements;
	vkGetImageMemoryRequirements(device, image, &requirements);
	return requirements;
}

uint32_t VulkanHelper::GetMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memoryProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		bool typeBitSet = (memoryTypeBits >> i) & 1;
		bool propertyPresent = (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties;

		if (typeBitSet && propertyPresent)
			return i;
	}

	ERROR("Failed to find suitable memory type index");

	return UINT32_MAX;
}

VkDeviceMemory VulkanHelper::AllocateDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex)
{
	VkMemoryAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocateInfo.allocationSize = size;
	allocateInfo.memoryTypeIndex = memoryTypeIndex;

	VkDeviceMemory memory;
	vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
	return memory;
}

VkDescriptorSetLayout VulkanHelper::CreateDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo descriptorSetCreateInfo = {};
	descriptorSetCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetCreateInfo.bindingCount = bindings.size();
	descriptorSetCreateInfo.pBindings = bindings.data();

	VkDescriptorSetLayout setLayout;
	vkCreateDescriptorSetLayout(device, &descriptorSetCreateInfo, nullptr, &setLayout);
	return setLayout;
}

VkDescriptorPool VulkanHelper::CreateDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
{
	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.maxSets = maxSets;
	createInfo.poolSizeCount = poolSizes.size();
	createInfo.pPoolSizes = poolSizes.data();

	VkDescriptorPool descriptorPool;
	vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);
	return descriptorPool;
}

VkDescriptorSet VulkanHelper::AllocateDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool)
{
	VkDescriptorSetAllocateInfo allocateInfo = {};
	allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &setLayout;
	allocateInfo.descriptorPool = descriptorPool;

	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);
	return descriptorSet;
}

VkImage VulkanHelper::CreateImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, const glm::uvec2& size)
{
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.arrayLayers = 1;
	createInfo.extent.width = size.x;
	createInfo.extent.height = size.y;
	createInfo.extent.depth = 1;
	createInfo.format = format;
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.mipLevels = 1;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = usage;

	VkImage image;
	vkCreateImage(device, &createInfo, nullptr, &image);
	return image;
}

VkSampler VulkanHelper::CreateSampler(VkDevice device, VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode)
{
	VkSamplerCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	createInfo.addressModeU = uAddressMode;
	createInfo.addressModeV = vAddressMode;
	createInfo.addressModeW = wAddressMode;
	createInfo.anisotropyEnable = VK_FALSE;
	createInfo.magFilter = magFilter;
	createInfo.minFilter = minFilter;

	VkSampler sampler;
	vkCreateSampler(device, &createInfo, nullptr, &sampler);
	return sampler;
}

