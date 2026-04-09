#pragma once


struct VulkanConverter
{
	static VkFormat GetFormat(ImageFormat format);
	static VkImageUsageFlags GetImageUsage(ImageUsage usage);
	static VkAttachmentLoadOp GetLoadOperation(LoadOperation loadOperation);
	static VkAttachmentStoreOp GetStoreOperation(StoreOperation storeOperation);
	static VkPipelineStageFlags GetPipelineStage(PipelineStage pipelineStage);
};
