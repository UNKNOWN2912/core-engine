#include <Engine.hpp>
#include <Renderer/Graphics.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <Core/Macro.hpp>
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <Vendor/stb/stb_image.h>

struct QueueIndices
{
	uint32_t graphic = UINT32_MAX, present = UINT32_MAX, transfer = UINT32_MAX, compute = UINT32_MAX;
};

struct Queues
{
	VkQueue graphic = VK_NULL_HANDLE, present = VK_NULL_HANDLE, transfer = VK_NULL_HANDLE, compute = VK_NULL_HANDLE;
};

struct UniformBufferData
{
	glm::mat4 model = glm::mat4(1.f), view = glm::mat4(1.f), projection = glm::mat4(1.f);
};

struct Vertex
{
	glm::vec3 position;
	glm::vec2 uv;
};

class Sandbox : public Application
{
	void Start() override
	{
		mViewport = {};
		mViewport.width = GetWindowRef().GetFrameBufferSize().x;
		mViewport.height = GetWindowRef().GetFrameBufferSize().y;
		mViewport.maxDepth = 1.f;
		mViewport.minDepth = 0.f;

		instance = CreateInstance();
		physicalDevice = GetPhysicalDevice(instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
		surface = CreateSurface(instance, GetWindowRef());
		queueIndices = GetQueueIndices(physicalDevice, surface);
		device = CreateDevice(instance, physicalDevice, queueIndices);
		queues = GetQueues(device, queueIndices);
		commandPool = CreateCommandPool(device);

		mRenderPass = CreateDefaultRenderPass(device);
		mCommandBuffer = AllocateCommandBuffer(device, commandPool);
		mImageAcquiredSemaphore = CreateSemaphore(device);
		mRenderingFinished = CreateSemaphore(device);

		mSwapchain = CreateSwapchain(physicalDevice, device, surface, mViewport, 0, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_PRESENT_MODE_FIFO_KHR);

		vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, nullptr);
		vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, mImages);

		for (uint32_t i = 0; i < mImageCount; i++)
		{
			mImageViews[i] = CreateImageView(device, mImages[i], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
			VkImageView attachment[] = { mImageViews[i] };
			mFrameBuffers[i] = CreateFrameBuffer(device, mRenderPass, mViewport.width, mViewport.height, attachment, 1);
		}

		mVertexShaderModule = CreateShaderModuleFromFile(device, "Shader/shader.vert.spv");
		mFragmentShaderModule = CreateShaderModuleFromFile(device, "Shader/shader.frag.spv");

		VkDescriptorSetLayoutBinding uniformSetBinding = {};
		uniformSetBinding.binding = 0;
		uniformSetBinding.descriptorCount = 1;
		uniformSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uniformSetBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding samplerSetBinding = {};
		samplerSetBinding.binding = 1;
		samplerSetBinding.descriptorCount = 1;
		samplerSetBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerSetBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayout setLayout = CreateDescriptorSetLayout(device, {uniformSetBinding, samplerSetBinding});

		mPipelineLayout = CreatePipelineLayout(device, {setLayout}, {});

		mGraphicPipeline = CreateGraphicPipeline(device, mRenderPass, mPipelineLayout, mVertexShaderModule, mFragmentShaderModule, 0);


		Vertex vertices[] =
		{
			{glm::vec3(0.5, 0.5, 0.0) , glm::vec2(0, 0)},
			{glm::vec3(0.5,-0.5, 0.0) , glm::vec2(0, 1)},
			{glm::vec3(-0.5,-0.5, 0.0), glm::vec2(1, 1)},
			{glm::vec3(-0.5, 0.5, 0.0), glm::vec2(1, 0)}
		};

		uint32_t indices[] =
		{
			0,1,2,2,3,0
		};

		mStagingVertexBuffer = CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		VkMemoryRequirements requirements = GetBufferMemoryRequirements(device, mStagingVertexBuffer);
		mStagingVertexBufferMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		vkBindBufferMemory(device, mStagingVertexBuffer, mStagingVertexBufferMemory, 0);
		vkMapMemory(device, mStagingVertexBufferMemory, 0, requirements.size, 0, &mVertexMap);

		memcpy(mVertexMap, vertices, sizeof(vertices));

		mVertexBuffer = CreateBuffer(device, sizeof(vertices), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		requirements = GetBufferMemoryRequirements(device, mVertexBuffer);
		mVertexBufferMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		vkBindBufferMemory(device, mVertexBuffer, mVertexBufferMemory, 0);

		VkCommandBuffer copyCommandBuffer = AllocateCommandBuffer(device, commandPool);


		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

		VkBufferCopy region = {};
		region.size = sizeof(vertices);
		vkCmdCopyBuffer(copyCommandBuffer, mStagingVertexBuffer, mVertexBuffer, 1, &region);

		vkEndCommandBuffer(copyCommandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyCommandBuffer;

		vkQueueSubmit(queues.transfer, 1, &submitInfo, VK_NULL_HANDLE);

		mStagingIndexBuffer = CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		requirements = GetBufferMemoryRequirements(device, mStagingIndexBuffer);
		mStagingIndexBufferMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		vkBindBufferMemory(device, mStagingIndexBuffer, mStagingIndexBufferMemory, 0);
		vkMapMemory(device, mStagingIndexBufferMemory, 0, requirements.size, 0, &mIndexMap);

		memcpy(mIndexMap, indices, sizeof(indices));

		mIndexBuffer = CreateBuffer(device, sizeof(indices), VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		requirements = GetBufferMemoryRequirements(device, mIndexBuffer);
		mIndexBufferMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		vkBindBufferMemory(device, mIndexBuffer, mIndexBufferMemory, 0);

		vkQueueWaitIdle(queues.transfer);

		vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

		region.size = sizeof(indices);
		vkCmdCopyBuffer(copyCommandBuffer, mStagingIndexBuffer, mIndexBuffer, 1, &region);

		vkEndCommandBuffer(copyCommandBuffer);

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyCommandBuffer;

		vkQueueSubmit(queues.transfer, 1, &submitInfo, VK_NULL_HANDLE);

		mUniformBuffer = CreateBuffer(device, sizeof(UniformBufferData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
		requirements = GetBufferMemoryRequirements(device, mUniformBuffer);
		mUniformBufferMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
		vkBindBufferMemory(device, mUniformBuffer, mUniformBufferMemory, 0);


		mUniformBufferData.model = glm::translate(glm::mat4(1.f), glm::vec3(0));
		mUniformBufferData.view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		mUniformBufferData.projection = glm::perspective(glm::radians(90.f), mViewport.width / mViewport.height, 0.01f, 100.f);

		vkMapMemory(device, mUniformBufferMemory, 0, sizeof(UniformBufferData), 0, &mUniformMap);
		memcpy(mUniformMap, &mUniformBufferData, sizeof(UniformBufferData));

		VkDescriptorPoolSize uniformPoolSize = {};
		uniformPoolSize.descriptorCount = 1;
		uniformPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		VkDescriptorPoolSize samplerPoolSize = {};
		samplerPoolSize.descriptorCount = 1;
		samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

		mDescriptorPool = CreateDescriptorPool(device, { uniformPoolSize, samplerPoolSize }, 1);

		mDescriptorSet = AllocateDescriptorSet(device, setLayout, mDescriptorPool);

		
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mUniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferData);

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.dstArrayElement = 0;
		write.dstBinding = 0;
		write.dstSet = mDescriptorSet;
		write.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);


		mSampler = CreateSampler(device, VK_FILTER_NEAREST, VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT);


		int width, height;
		stbi_uc* data = stbi_load("Texture/texture_01.png", &width, &height, nullptr, 4);

		glm::uvec2 size = { width, height };
		size_t byteSize = 4 * size.x * size.y;

		mStagingTextureBuffer = CreateBuffer(device, byteSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
		requirements = GetBufferMemoryRequirements(device, mStagingTextureBuffer);
		mStagingTextureMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));;
		vkBindBufferMemory(device, mStagingTextureBuffer, mStagingTextureMemory, 0);
		vkMapMemory(device, mStagingTextureMemory, 0, byteSize, 0, &mTextureMap);

		memcpy(mTextureMap, data, byteSize);

		mTextureImage = CreateImage(device, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, size);
		requirements = GetImageMemoryRequirements(device, mTextureImage);
		mTextureMemory = AllocateDeviceMemory(device, requirements.size, GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));
		vkBindImageMemory(device, mTextureImage, mTextureMemory, 0);
		mTextureImageView = CreateImageView(device, mTextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);

		vkQueueWaitIdle(queues.transfer);


		vkBeginCommandBuffer(copyCommandBuffer, &beginInfo);

		VkImageMemoryBarrier imageBarrier = {};
		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier.image = mTextureImage;
		imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = 1;
		imageBarrier.subresourceRange.layerCount = 1;


		vkCmdPipelineBarrier(copyCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);


		VkBufferImageCopy imageRegion = {};
		imageRegion.bufferImageHeight = 0;
		imageRegion.bufferRowLength = 0;
		imageRegion.bufferOffset = 0;
		imageRegion.imageOffset = { 0,0,0 };
		imageRegion.imageExtent = { size.x, size.y, 1 };
		imageRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageRegion.imageSubresource.baseArrayLayer = 0;
		imageRegion.imageSubresource.layerCount = 1;
		imageRegion.imageSubresource.mipLevel = 0;

		region.size = sizeof(indices);


		vkCmdCopyBufferToImage(copyCommandBuffer, mStagingTextureBuffer, mTextureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &imageRegion);

		imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageBarrier.image = mTextureImage;
		imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageBarrier.subresourceRange.baseArrayLayer = 0;
		imageBarrier.subresourceRange.baseMipLevel = 0;
		imageBarrier.subresourceRange.levelCount = 1;
		imageBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(copyCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrier);

		vkEndCommandBuffer(copyCommandBuffer);

		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &copyCommandBuffer;

		vkQueueSubmit(queues.transfer, 1, &submitInfo, VK_NULL_HANDLE);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = mTextureImageView;
		imageInfo.sampler = mSampler;

		write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.dstArrayElement = 0;
		write.dstBinding = 1;
		write.dstSet = mDescriptorSet;
		write.pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
	}


	void Update() override
	{

		uint32_t imageIndex;
		if (vkAcquireNextImageKHR(device, mSwapchain, UINT64_MAX, mImageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex) != VK_SUCCESS)
		{
			LOG("Resizing");
			ResizeViewport(GetWindowRef().GetFrameBufferSize());
			return;
		}


		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		vkBeginCommandBuffer(mCommandBuffer, &beginInfo);

		VkClearValue clearValue = { 0.01,0.01,0.01,1 };

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = mRenderPass;
		renderPassBeginInfo.renderArea.extent = { uint32_t(mViewport.width), uint32_t(mViewport.height) };
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.framebuffer = mFrameBuffers[imageIndex];
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearValue;

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer vertexBuffers[] = { mVertexBuffer };

		VkDeviceSize offsets[] = { 0 };



		vkCmdBindVertexBuffers(mCommandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(mCommandBuffer, mIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipelineLayout, 0, 1, &mDescriptorSet, 0, nullptr);


		VkRect2D scissor = {};
		scissor.extent = { uint32_t(mViewport.width), uint32_t(mViewport.height) };


		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicPipeline);

		vkCmdSetViewport(mCommandBuffer, 0, 1, &mViewport);
		vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(mCommandBuffer, 6, 1, 0, 0, 0);

		vkCmdEndRenderPass(mCommandBuffer);

		vkEndCommandBuffer(mCommandBuffer);

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffer;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAcquiredSemaphore;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mRenderingFinished;
		submitInfo.pWaitDstStageMask = waitStages;

		vkQueueSubmit(queues.graphic, 1, &submitInfo, VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &mSwapchain;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &mRenderingFinished;

		vkQueuePresentKHR(queues.present, &presentInfo);

		vkDeviceWaitIdle(device);
	}

	void End() override
	{
	}

	void ResizeViewport(const glm::uvec2& size)
	{
		vkDeviceWaitIdle(device);

		mViewport = {};
		mViewport.width = size.x;
		mViewport.height = size.y;
		mViewport.maxDepth = 1.f;
		mViewport.minDepth = 0.f;

		for (uint32_t i = 0; i < mImageCount; i++)
		{
			vkDestroyFramebuffer(device, mFrameBuffers[i], nullptr);
			vkDestroyImageView(device, mImageViews[i], nullptr);
		}
		vkDestroySwapchainKHR(device, mSwapchain, nullptr);

		mSwapchain = CreateSwapchain(physicalDevice, device, surface, mViewport, 0, VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_PRESENT_MODE_FIFO_KHR);

		vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, nullptr);
		vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, mImages);

		for (uint32_t i = 0; i < mImageCount; i++)
		{
			mImageViews[i] = CreateImageView(device, mImages[i], VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
			VkImageView attachment[] = { mImageViews[i] };
			mFrameBuffers[i] = CreateFrameBuffer(device, mRenderPass, mViewport.width, mViewport.height, attachment, 1);
		}

		mUniformBufferData.model = glm::translate(glm::mat4(1.f), glm::vec3(0));
		mUniformBufferData.view = glm::lookAt(glm::vec3(0, 0, -1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		mUniformBufferData.projection = glm::perspective(glm::radians(90.f), mViewport.width / mViewport.height, 0.01f, 100.f);


		memcpy(mUniformMap, &mUniformBufferData, sizeof(UniformBufferData));

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = mUniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferData);

		VkWriteDescriptorSet write = {};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.dstArrayElement = 0;
		write.dstBinding = 0;
		write.dstSet = mDescriptorSet;
		write.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);

	}


	VkInstance CreateInstance()
	{
		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		
		uint32_t extensionCount;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

		uint32_t layerCount = 1;
		const char* layers[] = {"VK_LAYER_KHRONOS_validation"};

		createInfo.enabledExtensionCount = extensionCount;
		createInfo.ppEnabledExtensionNames = extensions;

		createInfo.enabledLayerCount = layerCount;
		createInfo.ppEnabledLayerNames = layers;

		VkInstance instance;
		vkCreateInstance(&createInfo, nullptr, &instance);
		return instance;
	}

	VkPhysicalDevice GetPhysicalDevice(VkInstance instance, VkPhysicalDeviceType deviceType)
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
	
	VkSurfaceKHR CreateSurface(VkInstance instance, const Window& window)
	{
		assert(instance != VK_NULL_HANDLE);

		VkSurfaceKHR surface;
		glfwCreateWindowSurface(instance, (GLFWwindow*)window.GetNativeWindow(), nullptr, &surface);
		return surface;
	}

	QueueIndices GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
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

	Queues GetQueues(VkDevice device, const QueueIndices& queueIndices)
	{
		assert(device != VK_NULL_HANDLE);

		Queues queues;

		vkGetDeviceQueue(device, queueIndices.graphic, 0, &queues.graphic);
		vkGetDeviceQueue(device, queueIndices.present, 0, &queues.present);
		vkGetDeviceQueue(device, queueIndices.compute, 0, &queues.compute);
		vkGetDeviceQueue(device, queueIndices.transfer, 0, &queues.transfer);

		return queues;
	}

	VkDevice CreateDevice(VkInstance instance, VkPhysicalDevice physicalDevice, const QueueIndices& queueIndices)
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

	VkCommandPool CreateCommandPool(VkDevice device)
	{
		assert(device != VK_NULL_HANDLE);

		VkCommandPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		VkCommandPool commandPool;
		vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
		return commandPool;
	}

	VkSwapchainKHR CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkViewport viewport, int imageCount, VkFormat format, VkColorSpaceKHR colorSpace, VkPresentModeKHR presentMode)
	{
		uint32_t actualImageCount = imageCount;

		VkSurfaceCapabilitiesKHR capabilities;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &capabilities);

		if (imageCount == 0)
		{
			actualImageCount = ((capabilities.minImageCount + 1) <= capabilities.maxImageCount) ? capabilities.minImageCount + 1 : capabilities.minImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.imageArrayLayers = 1;
		createInfo.imageColorSpace = colorSpace;
		createInfo.imageExtent = { uint32_t(viewport.width), uint32_t(viewport.height) };
		createInfo.imageFormat = format;
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.minImageCount = actualImageCount;
		createInfo.presentMode = presentMode;
		createInfo.preTransform = capabilities.currentTransform;
		createInfo.surface = surface;

		VkSwapchainKHR swapchain;
		vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapchain);
		return swapchain;
	}

	VkSemaphore CreateSemaphore(VkDevice device)
	{
		VkSemaphoreCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkSemaphore semaphore;
		vkCreateSemaphore(device, &createInfo, nullptr, &semaphore);
		return semaphore;
	}

	VkFence CreateFence(VkDevice device)
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		VkFence fence;
		vkCreateFence(device, &createInfo, nullptr, &fence);
		return fence;
	}

	VkCommandBuffer AllocateCommandBuffer(VkDevice device, VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
		return commandBuffer;
	}
	VkRenderPass CreateDefaultRenderPass(VkDevice device)
	{

		VkAttachmentDescription swapchainImageAttachment = {};
		swapchainImageAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		swapchainImageAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
		swapchainImageAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		swapchainImageAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		swapchainImageAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		swapchainImageAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VkAttachmentReference swapchainAttachmentRef = {};
		swapchainAttachmentRef.attachment = 0;
		swapchainAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &swapchainAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = 1;
		createInfo.pAttachments = &swapchainImageAttachment;
		createInfo.subpassCount = 1;
		createInfo.pSubpasses = &subpass;
		createInfo.dependencyCount = 1;
		createInfo.pDependencies = &dependency;

		VkRenderPass renderPass;
		vkCreateRenderPass(device, &createInfo, nullptr, &renderPass);
		return renderPass;
	}

	VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect)
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

	VkFramebuffer CreateFrameBuffer(VkDevice device, VkRenderPass renderPass, uint32_t width, uint32_t height, const VkImageView attachments[], uint32_t attachmentCount)
	{
		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.attachmentCount = attachmentCount;
		createInfo.pAttachments = attachments;
		createInfo.width = width;
		createInfo.height = height;
		createInfo.layers = 1;
		createInfo.renderPass = renderPass;

		VkFramebuffer frameBuffer;
		vkCreateFramebuffer(device, &createInfo, nullptr, &frameBuffer);
		return frameBuffer;
	}

	VkShaderModule CreateShaderModuleFromFile(VkDevice device, std::string_view filename)
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

	VkPipelineLayout CreatePipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushConstants)
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

	VkPipeline CreateGraphicPipeline(VkDevice device, VkRenderPass renderPass, VkPipelineLayout pipelineLayout,  VkShaderModule vertexShaderModule, VkShaderModule fragmentShaderModule, uint32_t subpass)
	{
		VkViewport viewport = {};
		viewport.width = 800;
		viewport.height = 600;
		viewport.maxDepth = 1.f;
		viewport.minDepth = 0.f;

		VkRect2D scissor = {};
		scissor.extent = { 800, 600 };

		VkPipelineShaderStageCreateInfo shaderStages[2] = {};
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].module = vertexShaderModule;
		shaderStages[0].pName = "main";
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].module = fragmentShaderModule;
		shaderStages[1].pName = "main";
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.scissorCount = 1;
		viewportStateCreateInfo.pScissors = &scissor;

		VkVertexInputBindingDescription binding = {};
		binding.binding = 0;
		binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		binding.stride = sizeof(Vertex);

		VkVertexInputAttributeDescription attribute[2] = {};
		attribute[0].binding = 0;
		attribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute[0].location = 0;
		attribute[0].offset = offsetof(Vertex, position);

		attribute[1].binding = 0;
		attribute[1].format = VK_FORMAT_R32G32_SFLOAT;
		attribute[1].location = 1;
		attribute[1].offset = offsetof(Vertex, uv);

		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = 2;
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attribute;
		vertexInputStateCreateInfo.pVertexBindingDescriptions = &binding;

		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.lineWidth = 1.f;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;

		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		VkPipelineColorBlendAttachmentState attachment = {};
		attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.attachmentCount = 1;
		colorBlendStateCreateInfo.pAttachments = &attachment;

		VkPipelineDepthStencilStateCreateInfo depthStateCreateInfo = {};
		depthStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

		VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState);
		dynamicStateCreateInfo.pDynamicStates = dynamicStates;

		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.pColorBlendState = &colorBlendStateCreateInfo;
		createInfo.pDepthStencilState = &depthStateCreateInfo;
		createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		createInfo.pMultisampleState = &multisampleStateCreateInfo;
		createInfo.pRasterizationState = &rasterizationStateCreateInfo;
		createInfo.pVertexInputState = &vertexInputStateCreateInfo;
		createInfo.pViewportState = &viewportStateCreateInfo;
		createInfo.pDynamicState = &dynamicStateCreateInfo;
		createInfo.renderPass = renderPass;
		createInfo.subpass = subpass;
		createInfo.stageCount = 2;
		createInfo.pStages = shaderStages;
		createInfo.layout = pipelineLayout;

		VkPipeline pipeline = VK_NULL_HANDLE;
		vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &pipeline);
		return pipeline;
	}

	VkBuffer CreateBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage)
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

	VkMemoryRequirements GetBufferMemoryRequirements(VkDevice device, VkBuffer buffer)
	{
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(device, buffer, &requirements);
		return requirements;
	}

	VkMemoryRequirements GetImageMemoryRequirements(VkDevice device, VkImage image)
	{
		VkMemoryRequirements requirements;
		vkGetImageMemoryRequirements(device, image, &requirements);
		return requirements;
	}


	uint32_t GetMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags properties)
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

	VkDeviceMemory AllocateDeviceMemory(VkDevice device, VkDeviceSize size, uint32_t memoryTypeIndex)
	{
		VkMemoryAllocateInfo allocateInfo = {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = size;
		allocateInfo.memoryTypeIndex = memoryTypeIndex;

		VkDeviceMemory memory;
		vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
		return memory;
	}

	VkDescriptorSetLayout CreateDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		VkDescriptorSetLayoutCreateInfo descriptorSetCreateInfo = {};
		descriptorSetCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetCreateInfo.bindingCount = bindings.size();
		descriptorSetCreateInfo.pBindings = bindings.data();

		VkDescriptorSetLayout setLayout;
		vkCreateDescriptorSetLayout(device, &descriptorSetCreateInfo, nullptr, &setLayout);
		return setLayout;
	}

	VkDescriptorPool CreateDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxSets)
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

	VkDescriptorSet AllocateDescriptorSet(VkDevice device, VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool)
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

	VkImage CreateImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, const glm::uvec2& size)
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

	VkSampler CreateSampler(VkDevice device, VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode uAddressMode, VkSamplerAddressMode vAddressMode, VkSamplerAddressMode wAddressMode)
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

private:

	VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	QueueIndices queueIndices;
	Queues queues;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
	VkSemaphore mImageAcquiredSemaphore = VK_NULL_HANDLE;
	VkSemaphore mRenderingFinished = VK_NULL_HANDLE;
	VkRenderPass mRenderPass = VK_NULL_HANDLE;
	VkCommandBuffer mCommandBuffer = VK_NULL_HANDLE;
	VkPipeline mGraphicPipeline = VK_NULL_HANDLE;
	VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
	VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
	VkShaderModule mFragmentShaderModule = VK_NULL_HANDLE;

	uint32_t mImageCount = 0;
	VkImage mImages[8];
	VkImageView mImageViews[8];
	VkFramebuffer mFrameBuffers[8];

	VkBuffer mStagingVertexBuffer = VK_NULL_HANDLE;
	VkBuffer mVertexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mStagingVertexBufferMemory = VK_NULL_HANDLE;
	VkDeviceMemory mVertexBufferMemory = VK_NULL_HANDLE;
	void* mVertexMap = nullptr;

	VkBuffer mStagingIndexBuffer = VK_NULL_HANDLE;
	VkBuffer mIndexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mStagingIndexBufferMemory = VK_NULL_HANDLE;
	VkDeviceMemory mIndexBufferMemory = VK_NULL_HANDLE;
	void* mIndexMap = nullptr;

	VkBuffer mUniformBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mUniformBufferMemory = VK_NULL_HANDLE;
	void* mUniformMap = nullptr;

	UniformBufferData mUniformBufferData;
	VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
	VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
	VkViewport mViewport;


	VkBuffer mStagingTextureBuffer = VK_NULL_HANDLE;
	VkDeviceMemory mStagingTextureMemory = VK_NULL_HANDLE;
	void* mTextureMap = nullptr;

	VkImage mTextureImage = VK_NULL_HANDLE;
	VkImageView mTextureImageView = VK_NULL_HANDLE;
	VkDeviceMemory mTextureMemory = VK_NULL_HANDLE;
	VkSampler mSampler = VK_NULL_HANDLE;
};

Application *Application::Create() { return new Sandbox(); }