#pragma once
#include "Renderer/Types.hpp"
#include <cstdint>
#include <initializer_list>

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
        void AddAttachment(ImageFormat format, ImageLayout finalLayout, LoadOperation loadOp, StoreOperation storeOp, LoadOperation stencilLoadOp = LoadOperation::DontCare, StoreOperation stencilStoreOp = StoreOperation::DontCare, SampleCount sampleCount = SampleCount::One);
        void AddSubpass(std::initializer_list<uint32_t> colorAttachments, std::initializer_list<uint32_t> inputAttachments, uint32_t depthAttachment);
        void AddDependency(uint32_t sourceSubpass, uint32_t destinationSubpass, PipelineStage sourcePipelineStage, PipelineStage destinationPipelineStage);
    private:
};

inline void Test()
{
    RenderPass renderPass;
    renderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    renderPass.AddSubpass({0}, {1}, 2);
    renderPass.AddDependency(UINT32_MAX, 0, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests);
}