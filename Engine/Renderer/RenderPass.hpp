#pragma once
#include "Renderer/Types.hpp"
#include "glm/glm.hpp"
#include <cstdint>
#include <initializer_list>
#include <vector>
#include <vulkan/vulkan.h>

struct RenderPassAttachment
{
    ImageFormat format;
    ImageLayout finalLayout;
    LoadOperation loadOp;
    StoreOperation storeOp;
    LoadOperation stencilLoadOp;
    StoreOperation stencilStoreOp;
    SampleCount sampleCount;
};

class RenderPass
{
    public:
        const uint32_t ExternalSubpass = UINT32_MAX;
        
        void AddAttachment(ImageFormat format, ImageLayout finalLayout, LoadOperation loadOp, StoreOperation storeOp, LoadOperation stencilLoadOp = LoadOperation::DontCare, StoreOperation stencilStoreOp = StoreOperation::DontCare, SampleCount sampleCount = SampleCount::One);
        void AddSubpass(std::initializer_list<uint32_t> colorAttachments, std::initializer_list<uint32_t> inputAttachments, uint32_t depthAttachment = UINT32_MAX);
        void AddDependency(uint32_t sourceSubpass, uint32_t destinationSubpass, PipelineStage sourcePipelineStage, PipelineStage destinationPipelineStage);

        VkRenderPass GetHandle() const { return mHandle; }

        void CmdBeginRenderPass(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, const glm::uvec2& size, std::initializer_list<VkClearValue> clearValues);
        void CmdEndRenderPass(VkCommandBuffer commandBuffer);

        void Create();
        void Destroy();
    private:
        std::vector<VkAttachmentDescription> mAttachments; 
        std::vector<VkSubpassDescription> mSubpasses; 
        std::vector<VkSubpassDependency> mDependencies; 

        VkRenderPass mHandle = VK_NULL_HANDLE;
};