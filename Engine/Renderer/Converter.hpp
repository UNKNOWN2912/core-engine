#pragma once
#include "Renderer/Types.hpp"
#include <vulkan/vulkan.h>

VkFormat GetVulkanImageFormat(ImageFormat imageFormat);
VkImageLayout GetVulkanImageLayout(ImageLayout imageLayout);
VkImageUsageFlags GetVulkanImageUsage(ImageUsage imageUsage);
VkBufferUsageFlags GetVulkanBufferUsage(BufferUsage bufferUsage);
VkImageAspectFlags GetVulkanImageAspect(ImageAspect imageAspect);
VkAttachmentLoadOp GetVulkanLoadOperation(LoadOperation loadOperation);
VkAttachmentStoreOp GetVulkanStoreOperation(StoreOperation storeOperation);
VkDescriptorType GetVulkanDescriptorType(DescriptorType descriptorType);
VkPipelineStageFlags GetVulkanPipelineStage(PipelineStage pipelineStage);
VkShaderStageFlags GetVulkanShaderStage(ShaderStage shaderStage);
VkSampleCountFlags GetVulkanSampleCount(SampleCount sampleCount);
VkMemoryPropertyFlags GetVulkanMemoryProperty(MemoryProperty memoryProperty);
VkPhysicalDeviceType GetVulkanDeviceType(DeviceType deviceType);
VkFilter GetVulkanFilter(Filter filter);
VkSamplerAddressMode GetVulkanAddressMode(AddressMode addressMode);
