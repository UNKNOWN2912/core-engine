#pragma once
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Types.hpp"
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

class Subpass
{
public:
    void AddColorAttachment(uint32_t index);
    void AddInputAttachment(uint32_t index);
    void AddResolveAttachment(uint32_t index);

    void SetDepthAttachment(uint32_t index);
    void SetDepthResolveAttachment(uint32_t index);

private:
    friend class RenderPass;

    std::vector<VkAttachmentReference2> mColorAttachmentReferences;
    std::vector<VkAttachmentReference2> mResolveAttachmentReferences;
    std::vector<VkAttachmentReference2> mInputAttachmentReferences;
    VkAttachmentReference2 mDepthAttachmentReference;
    VkAttachmentReference2 mDepthResolveAttachmentReference;
    VkSubpassDescriptionDepthStencilResolve mDepthResolveDescription;

    VkSubpassDescription2
    GetSubpassDescription(PipelineBindPoint bindPoint) const;
};

class RenderPass
{
public:
    RenderPass() = default;
    RenderPass(const RenderPass &) = delete;
    RenderPass &operator=(const RenderPass &renderPass) = delete;
    RenderPass(RenderPass &&renderPass) noexcept
        : mAttachments(std::move(renderPass.mAttachments)), mSubpasses(std::move(renderPass.mSubpasses)),
          mDependencies(std::move(renderPass.mDependencies)), mHandle(renderPass.mHandle)
    {
        renderPass.mHandle = VK_NULL_HANDLE;
    }
    RenderPass &operator=(RenderPass &&renderPass) noexcept
    {
        DestroyRenderPass();

        mAttachments = std::move(renderPass.mAttachments);
        mSubpasses = std::move(renderPass.mSubpasses);
        mDependencies = std::move(renderPass.mDependencies);

        mHandle = renderPass.mHandle;
        renderPass.mHandle = VK_NULL_HANDLE;

        return *this;
    }
    ~RenderPass();

    static const uint32_t ExternalSubpass = UINT32_MAX;
    uint32_t AddAttachment(ImageFormat format, ImageLayout initialLayout, ImageLayout finalLayout, LoadOperation loadOp, StoreOperation storeOp, LoadOperation stencilLoadOp = LoadOperation::DontCare, StoreOperation stencilStoreOp = StoreOperation::DontCare, SampleCount sampleCount = SampleCount::One);
    void AddSubpass(std::initializer_list<uint32_t> colorAttachments, std::initializer_list<uint32_t> inputAttachments, std::initializer_list<uint32_t> resolveAttachments, uint32_t depthAttachment, PipelineBindPoint bindPoint);
    void AddDependency(uint32_t sourceSubpass, uint32_t destinationSubpass, PipelineStage sourcePipelineStage, PipelineStage destinationPipelineStage);

    void AddSubpass(const Subpass &subpass, PipelineBindPoint bindPoint);

    VkRenderPass GetHandle() const
    {
        return mHandle;
    }

    void CmdBeginRenderPass(const CommandBuffer &commandBuffer, const FrameBuffer &frameBuffer, const glm::uvec2 &size, std::initializer_list<VkClearValue> clearValues);
    void CmdEndRenderPass(const CommandBuffer &commandBuffer);

    void CreateRenderPass();
    void DestroyRenderPass();

private:
    std::vector<VkAttachmentDescription2> mAttachments;
    std::vector<VkSubpassDescription2> mSubpasses;
    std::vector<VkSubpassDependency2> mDependencies;

    VkRenderPass mHandle = VK_NULL_HANDLE;
};