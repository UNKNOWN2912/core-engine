#include <Renderer/Vulkan/VulkanUtility.hpp>
#include <cstdio>
#include <print>


VkAttachmentDescription populateAttachmentDescription(VkFormat format, VkImageLayout finalLayout, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkSampleCountFlagBits samples)
{
	VkAttachmentDescription attachment = {};
	attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachment.finalLayout = finalLayout;
	attachment.format = format;
	attachment.samples = samples;
	attachment.loadOp = loadOp;
	attachment.storeOp = storeOp;
	attachment.stencilLoadOp = stencilLoadOp;
	attachment.stencilStoreOp = stencilStoreOp;

	return attachment;
}

VkAttachmentReference populateAttachmentReference(uint32_t index, VkImageLayout imageLayout)
{
	VkAttachmentReference reference = {};
	reference.attachment = index;
	reference.layout = imageLayout;

	return reference;
}

void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue transferQueue, uint32_t width, uint32_t height, VkBuffer buffer, VkImage image, VkImageAspectFlags aspectMask)
{
	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferImageHeight = 0;
	region.bufferRowLength = 0;
	region.imageExtent.width = width;
	region.imageExtent.height = height;
	region.imageExtent.depth = 1;
	region.imageOffset = {};
	region.imageSubresource.aspectMask = aspectMask;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageSubresource.mipLevel = 0;

	VkCommandBuffer transferCommandBuffer = allocateCommandBuffers(device, commandPool);

	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);
	vkCmdCopyBufferToImage(transferCommandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
	vkEndCommandBuffer(transferCommandBuffer);

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transferCommandBuffer;

	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);

	vkQueueWaitIdle(transferQueue);
}

void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue transferQueue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask)
{
	VkCommandBuffer transitionCommandBuffer = allocateCommandBuffers(device, commandPool);

	VkImageMemoryBarrier barrier = { VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	barrier.image = image;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
	barrier.subresourceRange.aspectMask = aspectMask;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.layerCount = 1;
	barrier.subresourceRange.levelCount = 1;

	VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(transitionCommandBuffer, &beginInfo);
	vkCmdPipelineBarrier(transitionCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);
	vkEndCommandBuffer(transitionCommandBuffer);

	VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &transitionCommandBuffer;

	vkQueueSubmit(transferQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(transferQueue);
}

VkSubpassDescription populateSubpassDescription(const std::vector<VkAttachmentReference>& colorAttachments, const std::vector<VkAttachmentReference>& inputAttachments, const VkAttachmentReference& depthAttachment, VkPipelineBindPoint pipelineBindPoint)
{
	VkSubpassDescription description = {};
	description.colorAttachmentCount = colorAttachments.size();
	description.pColorAttachments = colorAttachments.data();
	description.pDepthStencilAttachment = &depthAttachment;
	description.pipelineBindPoint = pipelineBindPoint;
	description.inputAttachmentCount = inputAttachments.size();
	description.pInputAttachments = inputAttachments.data();

	return description;
}

VkSubpassDescription populateSubpassDescription(const std::vector<VkAttachmentReference>& colorAttachments, const std::vector<VkAttachmentReference>& inputAttachments, VkPipelineBindPoint pipelineBindPoint)
{
	VkSubpassDescription description = {};
	description.colorAttachmentCount = colorAttachments.size();
	description.pColorAttachments = colorAttachments.data();
	description.inputAttachmentCount = inputAttachments.size();
	description.pInputAttachments = inputAttachments.data();
	description.pipelineBindPoint = pipelineBindPoint;

	return description;
}

VkSubpassDependency populateSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
{
	VkSubpassDependency subpassDependencies = {};
	subpassDependencies.srcSubpass = srcSubpass;
	subpassDependencies.dstSubpass = dstSubpass;
	subpassDependencies.srcStageMask = srcStageMask;
	subpassDependencies.dstStageMask = dstStageMask;
	subpassDependencies.srcAccessMask = srcAccessMask;
	subpassDependencies.dstAccessMask = dstAccessMask;

	return subpassDependencies;
}

VkRenderPass createRenderPass(VkDevice device)
{
	VkRenderPassCreateInfo createInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO };

	VkAttachmentDescription colorAttachmentDescription = populateAttachmentDescription(VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_STORE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_SAMPLE_COUNT_1_BIT);
	// colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// colorAttachmentDescription.format = VK_FORMAT_B8G8R8A8_SRGB;
	// colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	// colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

	VkAttachmentDescription depthAttachmentDescription = populateAttachmentDescription(VK_FORMAT_D32_SFLOAT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, VK_SAMPLE_COUNT_1_BIT);

	// depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	// depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	// depthAttachmentDescription.format = VK_FORMAT_D32_SFLOAT;
	// depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	// depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	// depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	VkAttachmentReference colorAttachmentReference = populateAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
	// colorAttachmentReference.attachment = 0;
	// colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentReference = populateAttachmentReference(1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	// depthAttachmentReference.attachment = 1;
	// depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	// VkAttachmentReference colorAttachmentRefs[] = {colorAttachmentReference};

	std::vector<VkAttachmentReference> colorAttachmentRefs = { colorAttachmentReference };

	VkSubpassDescription subpassDescription = populateSubpassDescription(colorAttachmentRefs, {}, depthAttachmentReference, VK_PIPELINE_BIND_POINT_GRAPHICS);
	// subpassDescription.colorAttachmentCount = sizeof(colorAttachmentRefs) / sizeof(VkAttachmentReference);
	// subpassDescription.pColorAttachments = colorAttachmentRefs;
	// subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
	// subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	VkSubpassDependency subpassDependencies = populateSubpassDependency(VK_SUBPASS_EXTERNAL, 0, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT);
	// subpassDependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
	// subpassDependencies.dstSubpass = 0;
	// subpassDependencies.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	// subpassDependencies.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	// subpassDependencies.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	// subpassDependencies.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

	VkSubpassDescription subpassDescriptions[] = { subpassDescription };
	VkSubpassDependency dependencies[] = { subpassDependencies };

	VkAttachmentDescription attachments[] = { colorAttachmentDescription, depthAttachmentDescription };

	createInfo.subpassCount = sizeof(subpassDescriptions) / sizeof(VkSubpassDescription);
	createInfo.dependencyCount = sizeof(dependencies) / sizeof(VkSubpassDependency);
	createInfo.attachmentCount = sizeof(attachments) / sizeof(VkAttachmentDescription);
	createInfo.pSubpasses = &subpassDescription;
	createInfo.pDependencies = dependencies;
	createInfo.pAttachments = attachments;

	VkRenderPass renderPass;
	vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);
	return renderPass;
}
VkCommandPool createCommandPool(VkDevice device)
{
	VkCommandPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCommandPool commandPool;
	vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
	return commandPool;
}
VkDescriptorPool createDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxAllocation)
{
	VkDescriptorPoolCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
	createInfo.maxSets = maxAllocation;
	createInfo.poolSizeCount = poolSizes.size();
	createInfo.pPoolSizes = poolSizes.data();

	VkDescriptorPool descriptorPool;
	vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool);
	return descriptorPool;
}

VkCommandBuffer allocateCommandBuffers(VkDevice device, VkCommandPool commandPool, VkBool32 secondary)
{
	VkCommandBufferAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
	allocateInfo.commandBufferCount = 1;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
	return commandBuffer;
}

VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
	VkImageViewCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	createInfo.image = image;
	createInfo.format = format;
	createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	createInfo.subresourceRange.aspectMask = aspectMask;
	createInfo.subresourceRange.baseArrayLayer = 0;
	createInfo.subresourceRange.baseMipLevel = 0;
	createInfo.subresourceRange.layerCount = 1;
	createInfo.subresourceRange.levelCount = 1;

	VkImageView imageView;
	vkCreateImageView(device, &createInfo, nullptr, &imageView);
	return imageView;
}

VkSemaphore createSemaphore(VkDevice device)
{
	VkSemaphoreCreateInfo createInfo = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };

	VkSemaphore semaphore;
	vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
	return semaphore;
}

VkFence createFence(VkDevice device, VkBool32 signaled)
{
	VkFenceCreateInfo createInfo = { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	createInfo.flags = signaled == VK_TRUE ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

	VkFence fence;
	vkCreateFence(device, &createInfo, nullptr, &fence);
	return fence;
}

VkFramebuffer createFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& imageViewAttachments)
{
	VkFramebufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };
	createInfo.attachmentCount = imageViewAttachments.size();
	createInfo.pAttachments = imageViewAttachments.data();
	createInfo.width = extent.width;
	createInfo.height = extent.height;
	createInfo.renderPass = renderPass;
	createInfo.layers = 1;

	VkFramebuffer frameBuffer;
	vkCreateFramebuffer(device, &createInfo, nullptr, &frameBuffer);
	return frameBuffer;
}

VkShaderModule createShaderModuleFromMemory(VkDevice device, const std::vector<uint32_t>& code)
{
	VkShaderModuleCreateInfo createInfo = { VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
	createInfo.codeSize = code.size() * sizeof(uint32_t);
	createInfo.pCode = code.data();

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	return shaderModule;
}

VkShaderModule createShaderFromFile(VkDevice device, const char* filename)
{
	std::vector<uint32_t> code;
	FILE* fp = fopen(filename, "rb");
	if (fp == nullptr)
	{
		std::println("Failed to find file: {}", filename);
	}

	fseek(fp, 0L, SEEK_END);
	int64_t size = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	code.resize(size / sizeof(uint32_t));
	fread(code.data(), size, 1, fp);
	fclose(fp);

	return createShaderModuleFromMemory(device, code);
}

VkPipelineLayout createPipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushContantRanges)
{
	VkPipelineLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	createInfo.setLayoutCount = setLayouts.size();
	createInfo.pSetLayouts = setLayouts.data();

	createInfo.pushConstantRangeCount = pushContantRanges.size();
	createInfo.pPushConstantRanges = pushContantRanges.data();

	VkPipelineLayout pipelineLayout;
	vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout);
	return pipelineLayout;
}

uint32_t getMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperty)
{
	VkPhysicalDeviceMemoryProperties properties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

	for (int i = 0; i < properties.memoryTypeCount; i++)
	{
		if (memoryTypeBits & (1 << i) && (properties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty)
		{
			return i;
		}
	}

	return UINT32_MAX;
}

VkBuffer createBuffer(VkDevice device, VkBufferUsageFlags usage, VkDeviceSize size)
{
	VkBufferCreateInfo createInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.usage = usage;
	createInfo.size = size;

	VkBuffer buffer;
	vkCreateBuffer(device, &createInfo, nullptr, &buffer);
	return buffer;
}

VkSampler createSampler(VkDevice device, VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode addressMode, uint32_t anisotropy)
{
	VkSamplerCreateInfo createInfo = { VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };
	createInfo.addressModeU = addressMode;
	createInfo.addressModeV = addressMode;
	createInfo.addressModeW = addressMode;
	createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	createInfo.anisotropyEnable = anisotropy == 0 ? VK_FALSE : VK_TRUE;
	createInfo.maxAnisotropy = anisotropy;
	createInfo.compareEnable = VK_FALSE;
	createInfo.magFilter = magFilter;
	createInfo.minFilter = minFilter;
	createInfo.maxLod = 1;
	createInfo.minLod = 0;
	createInfo.mipLodBias = 1.f;
	createInfo.unnormalizedCoordinates = VK_FALSE;

	VkSampler sampler;
	vkCreateSampler(device, &createInfo, nullptr, &sampler);
	return sampler;
}

VkDeviceMemory allocateMemoryForBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer buffer, VkMemoryPropertyFlags memoryProperty)
{
	VkMemoryRequirements requirements;
	vkGetBufferMemoryRequirements(device, buffer, &requirements);

	VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocateInfo.allocationSize = requirements.size;
	allocateInfo.memoryTypeIndex = getMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, memoryProperty);

	VkDeviceMemory memory;
	vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
	vkBindBufferMemory(device, buffer, memory, 0);
	return memory;
}

VkDeviceMemory allocateMemoryForImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImage image, VkMemoryPropertyFlags memoryProperty)
{
	VkMemoryRequirements requirements;
	vkGetImageMemoryRequirements(device, image, &requirements);

	VkMemoryAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
	allocateInfo.allocationSize = requirements.size;
	allocateInfo.memoryTypeIndex = getMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, memoryProperty);

	VkDeviceMemory memory;
	vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
	vkBindImageMemory(device, image, memory, 0);
	return memory;
}

VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
	VkDescriptorSetLayoutCreateInfo createInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	createInfo.bindingCount = bindings.size();
	createInfo.pBindings = bindings.data();

	VkDescriptorSetLayout setLayout;
	vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &setLayout);
	return setLayout;
}

VkImage createImage2D(VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits samples)
{
	VkImageCreateInfo createInfo = { VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	createInfo.imageType = VK_IMAGE_TYPE_2D;
	createInfo.arrayLayers = 1;
	createInfo.extent.width = width;
	createInfo.extent.height = height;
	createInfo.extent.depth = 1;
	createInfo.format = format;
	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.mipLevels = 1;
	createInfo.samples = samples;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.usage = usage;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;

	VkImage image;
	vkCreateImage(device, &createInfo, nullptr, &image);
	return image;
}

VkDescriptorSet allocateDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout setLayout)
{
	VkDescriptorSetAllocateInfo allocateInfo = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	allocateInfo.descriptorPool = descriptorPool;
	allocateInfo.descriptorSetCount = 1;
	allocateInfo.pSetLayouts = &setLayout;

	VkDescriptorSet descriptorSet;
	vkAllocateDescriptorSets(device, &allocateInfo, &descriptorSet);
	return descriptorSet;
}

void submitCommandBuffer(VkQueue queue, VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkPipelineStageFlags waitFlag)
{
	VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
	submitInfo.pSignalSemaphores = &signalSemaphore;
	submitInfo.pWaitSemaphores = &waitSemaphore;
	submitInfo.pWaitDstStageMask = &waitFlag;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.commandBufferCount = 1;

	vkQueueSubmit(queue, 1, &submitInfo, fence);
}

void freeCommandBuffers(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer)
{
	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
void destroyInstance(VkInstance instance)
{
	vkDestroyInstance(instance, nullptr);
}
void destroyLogicalDevice(VkDevice device)
{
	vkDestroyDevice(device, nullptr);
}
void destroyRenderPass(VkDevice device, VkRenderPass renderPass)
{
	vkDestroyRenderPass(device, renderPass, nullptr);
}
void destroyCommandPool(VkDevice device, VkCommandPool commandpool)
{
	vkDestroyCommandPool(device, commandpool, nullptr);
}
void destroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorpool)
{
	vkDestroyDescriptorPool(device, descriptorpool, nullptr);
}
void destroyGraphicPipeline(VkDevice device, VkPipeline pipeline)
{
	vkDestroyPipeline(device, pipeline, nullptr);
}
void destroyWindowSurface(VkInstance instance, VkSurfaceKHR surface)
{
	vkDestroySurfaceKHR(instance, surface, nullptr);
}