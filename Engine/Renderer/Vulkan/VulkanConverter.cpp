#include "VulkanConverter.hpp"
#include <Renderer/Enum.hpp>
#include <vulkan/vulkan.h>
#include <Core/Macro.hpp>


VkFormat VulkanConverter::GetFormat(ImageFormat format)
{
	switch (format)
	{
	case ImageFormat::None:
		return VK_FORMAT_UNDEFINED;
		break;
	case ImageFormat::R8:
		return VK_FORMAT_R8_SRGB;
			break;
	case ImageFormat::RG8:
		return VK_FORMAT_R8G8_SRGB;
			break;
	case ImageFormat::RGB8:
		return VK_FORMAT_R8G8B8_SRGB;
			break;
	case ImageFormat::RGBA8:
		return VK_FORMAT_R8G8B8A8_SRGB;
			break;
	case ImageFormat::R16:
		return VK_FORMAT_R16_SFLOAT;
			break;
	case ImageFormat::RG16:
		return VK_FORMAT_R16G16_SFLOAT;
			break;
	case ImageFormat::RGB16:
		return VK_FORMAT_R16G16B16_SFLOAT;
			break;
	case ImageFormat::RGBA16:
		return VK_FORMAT_R16G16B16A16_SFLOAT;
			break;
	case ImageFormat::R32:
		return VK_FORMAT_R32_SFLOAT;
			break;
	case ImageFormat::RG32:
		return VK_FORMAT_R32G32_SFLOAT;
			break;
	case ImageFormat::RGB32:
		return VK_FORMAT_R32G32B32_SFLOAT;
			break;
	case ImageFormat::RGBA32:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
	case ImageFormat::R64:
		return VK_FORMAT_R64_SFLOAT;
			break;
	case ImageFormat::RG64:
		return VK_FORMAT_R64G64_SFLOAT;
			break;
	case ImageFormat::RGB64:
		return VK_FORMAT_R64G64B64_SFLOAT;
			break;
	case ImageFormat::RGBA64:
		return VK_FORMAT_R64G64B64A64_SFLOAT;
			break;
	case ImageFormat::BGRA8:
		return VK_FORMAT_B8G8R8A8_SRGB;
		break;
	case ImageFormat::D32:
		return VK_FORMAT_D32_SFLOAT;
		break;
	}

	ERROR("Invalid format");

	return VK_FORMAT_UNDEFINED;
}

VkImageUsageFlags VulkanConverter::GetImageUsage(ImageUsage usage)
{
	std::pair<VkImageUsageFlags, ImageUsage> usageMap[] =
	{
		{ 0, ImageUsage::None},
		{ VK_IMAGE_USAGE_SAMPLED_BIT, ImageUsage::Sampler },
		{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, ImageUsage::ColorAttachment },
		{ VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, ImageUsage::InputAttachment },
		{ VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, ImageUsage::Presentation},
		{ VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, ImageUsage::DepthAttachment }
	};

	VkImageUsageFlags result = 0;

	for (auto [vkUsage, usg] : usageMap)
	{
		if (uint8_t(usage & usg))
		{
			result |= vkUsage;
		}
	}

	return result;
}

VkAttachmentLoadOp VulkanConverter::GetLoadOperation(LoadOperation loadOperation)
{
	switch (loadOperation)
	{
	case LoadOperation::None:
		return VK_ATTACHMENT_LOAD_OP_NONE;
		break;
	case LoadOperation::Load:
		return VK_ATTACHMENT_LOAD_OP_LOAD;
		break;
	case LoadOperation::Clear:
		return VK_ATTACHMENT_LOAD_OP_CLEAR;
		break;
	case LoadOperation::DontCare:
		return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		break;
	}

	ERROR("Invalid load operation");

	return VK_ATTACHMENT_LOAD_OP_NONE;
}

VkAttachmentStoreOp VulkanConverter::GetStoreOperation(StoreOperation storeOperation)
{
	switch (storeOperation)
	{
	case StoreOperation::None:
		return VK_ATTACHMENT_STORE_OP_NONE;	
		break;
	case StoreOperation::Store:
		return VK_ATTACHMENT_STORE_OP_STORE;	
		break;
	case StoreOperation::DontCare:
		return VK_ATTACHMENT_STORE_OP_DONT_CARE;	
		break;
	}

	ERROR("Invalid store operation");

	return VK_ATTACHMENT_STORE_OP_NONE;
}

VkPipelineStageFlags VulkanConverter::GetPipelineStage(PipelineStage pipelineStage)
{
	switch (pipelineStage)
	{
	case PipelineStage::None:
		return VK_PIPELINE_STAGE_NONE;
		break;
	case PipelineStage::Top:
		return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
	case PipelineStage::Bottom:
		return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
	case PipelineStage::Transfer:
		return VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case PipelineStage::ColorOutput:
		return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
	case PipelineStage::EarlyFragmentTest:
		return VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		break;
	case PipelineStage::LateFragmentTest:
		return VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		break;
	}

	ERROR("Invalid pipeline stage");

	return VK_PIPELINE_STAGE_NONE;
}
