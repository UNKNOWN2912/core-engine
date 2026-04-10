#pragma once
#include "Renderer/Enum.hpp"

struct RenderPassData;

class RenderPass
{
public:
	void AddAttachment(ImageFormat format, ImageUsage usage, LoadOperation loadOperation, StoreOperation storeOperation);
	void AddSubpass(const std::vector<uint32_t>& colorOutputRef, const std::vector<uint32_t>& inputAttachmentRef, uint32_t depthAttachment);
	void AddDependency(uint32_t srcSubpass, uint32_t dstSubpass, PipelineStage srcStage, PipelineStage dstStage);
	void Create();

	void CmdBeginRenderPass();
	void CmdEndRenderPass();
private:
	RenderPassData mData;
};
