#include <Renderer/RenderPass.hpp>
#include <vulkan/vulkan.h>
#include <vector>
#include "VulkanConverter.hpp"
#include <Renderer/Graphics.hpp>
#include "VulkanTypes.hpp"

struct RenderPassData
{
	std::vector<VkAttachmentDescription> attachments;
	std::vector<VkSubpassDescription> subpasses;
	std::vector<VkSubpassDependency> dependencies;

	VkRenderPass handle;
};


void RenderPass::AddAttachment(ImageFormat format, ImageUsage usage, LoadOperation loadOperation, StoreOperation storeOperation)
{
	VkAttachmentDescription description = {};
	description.format = VulkanConverter::GetFormat(format);
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.loadOp = VulkanConverter::GetLoadOperation(loadOperation);
	description.storeOp = VulkanConverter::GetStoreOperation(storeOperation);
	description.samples = VK_SAMPLE_COUNT_1_BIT;

	switch (usage)
	{
	case ImageUsage::None:
		description.finalLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		break;
	case ImageUsage::Sampler:
		description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case ImageUsage::Presentation:
		description.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		break;
	case ImageUsage::ColorAttachment:
		description.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		break;
	case ImageUsage::InputAttachment:
		description.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		break;
	case ImageUsage::DepthAttachment:
		description.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		break;
	}

	mData.attachments.push_back(description);
}

void RenderPass::AddSubpass(const std::vector<uint32_t>& colorOutputRef, const std::vector<uint32_t>& inputAttachmentRef, uint32_t depthAttachment)
{
	std::vector<VkAttachmentReference> colorRef;
	std::vector<VkAttachmentReference> inputRef;
	VkAttachmentReference depthRef = {};

	for (size_t i = 0; i < colorOutputRef.size(); i++)
	{
		VkAttachmentReference reference = {};
		reference.attachment = colorOutputRef[i];
		reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		colorRef.push_back(reference);
	}

	for (size_t i = 0; i < inputAttachmentRef.size(); i++)
	{
		VkAttachmentReference reference = {};
		reference.attachment = inputAttachmentRef[i];
		reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		colorRef.push_back(reference);
	}

	if (depthAttachment != UINT32_MAX)
	{
		VkAttachmentReference reference = {};
		reference.attachment = depthAttachment;
		reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		depthRef = reference;
	}

	VkSubpassDescription description = {};

	description.colorAttachmentCount = colorRef.size();
	description.pColorAttachments = colorRef.data();

	description.inputAttachmentCount = inputRef.size();
	description.pInputAttachments = inputRef.data();

	description.pDepthStencilAttachment = &depthRef;

	mData.subpasses.push_back(description);
	

}

void RenderPass::AddDependency(uint32_t srcSubpass, uint32_t dstSubpass, PipelineStage srcStage, PipelineStage dstStage)
{
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = (srcSubpass == UINT32_MAX) ? VK_SUBPASS_EXTERNAL : srcSubpass;
	dependency.dstSubpass = (dstSubpass == UINT32_MAX) ? VK_SUBPASS_EXTERNAL : dstSubpass;
	dependency.srcStageMask = VulkanConverter::GetPipelineStage(srcStage);
	dependency.dstStageMask = VulkanConverter::GetPipelineStage(dstStage);

	switch (srcStage)
	{
	case PipelineStage::None:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Top:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Bottom:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Transfer:
		dependency.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case PipelineStage::ColorOutput:
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		break;
	case PipelineStage::EarlyFragmentTest:
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		break;
	case PipelineStage::LateFragmentTest:
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		break;
	}

	switch (dstStage)
	{
	case PipelineStage::None:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Top:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Bottom:
		dependency.srcAccessMask = VK_ACCESS_NONE;
		break;
	case PipelineStage::Transfer:
		dependency.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
	case PipelineStage::ColorOutput:
		dependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case PipelineStage::EarlyFragmentTest:
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case PipelineStage::LateFragmentTest:
		dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	}

	mData.dependencies.push_back(dependency);
}

void RenderPass::Create()
{
	VkRenderPassCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;

	createInfo.attachmentCount = mData.attachments.size();
	createInfo.pAttachments = mData.attachments.data();

	createInfo.subpassCount = mData.subpasses.size();
	createInfo.pSubpasses = mData.subpasses.data();

	createInfo.dependencyCount = mData.dependencies.size();
	createInfo.pDependencies = mData.dependencies.data();
	
	vkCreateRenderPass(Graphics::GetGraphicsData().device, &createInfo, nullptr, &mData.handle);
}
