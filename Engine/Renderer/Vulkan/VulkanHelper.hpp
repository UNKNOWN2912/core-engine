#pragma once
#include "Core/Window.hpp"
#include "Renderer/Enums.hpp"
#include "Renderer/GraphicPipeline.hpp"
#include "Renderer/PipelineLayout.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Vulkan/VulkanSwapchain.hpp"
#include <vulkan/vulkan.h>

struct QueueIndex
{
	uint32_t graphic = UINT32_MAX;
	uint32_t transfer = UINT32_MAX;
	uint32_t present = UINT32_MAX;
	uint32_t compute = UINT32_MAX;
};

struct Queue
{
	VkQueue graphic = VK_NULL_HANDLE;
	VkQueue transfer = VK_NULL_HANDLE;
	VkQueue present = VK_NULL_HANDLE;
	VkQueue compute = VK_NULL_HANDLE;
};

struct VulkanHelper
{
    static VkInstance CreateInstance(bool enableValidation = false);
    static VkPhysicalDevice GetPhysicalDevice(VkInstance instance);
    static VkDevice CreateDevice(VkPhysicalDevice physicalDevice, const QueueIndex& queueIndices);
    static VkCommandPool CreateCommandPool(VkDevice device);
    static VulkanSwapchain CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const Window& window);
    static QueueIndex GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static Queue GetQueues(VkDevice device, const QueueIndex& queueIndices);
    static VkSurfaceKHR CreateSurface(VkInstance instance, const Window& window);

    static VkFormat ConvertToVulkanFormat(ImageFormat format);
    static VkImageLayout GetLayoutFromAttachmentUsage(ImageUsage usage);
    static VkAttachmentLoadOp ConvertToVulkanLoadOperation(LoadOperation loadOperation);
    static VkAttachmentStoreOp ConvertToVulkanStoreOperation(StoreOperation storeOperation);

    static VkAttachmentDescription GetVulkanAttachmentFromAttachment(const Attachment& attachment);

    static uint32_t GetMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperty);
    static VkDeviceMemory AllocateMemoryForImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImage image, VkMemoryPropertyFlags memoryProperty);

    static VkShaderStageFlags ConvertToVulkanShaderStage(ShaderType shaderType);
    static VkDescriptorType ConvertToVulkanDescriptorType(DescriptorType descriptorType);
    static VkPrimitiveTopology ConvertToVulkanPrimitiveTopology(PrimitiveType primitiveType);
    static VkCullModeFlags ConvertToVulkanCullMode(CullMode cullMode);
    static VkFrontFace ConvertToVulkanFrontFace(FrontFace frontFace);
    static VkFormat ConvertToVulkanVertexFormat(VertexFormatType format);
    static VkSampleCountFlagBits ConvertToVulkanSampleCount(uint32_t sampleCount);
};