#pragma once
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Utility.hpp"

struct RenderCommand
{
    const Buffer *vertexBuffer = nullptr;
    const Buffer *indexBuffer = nullptr;
    uint32_t indexCount = 0;

    const InstanceBuffer *instanceBuffer = nullptr;
    uint32_t instanceCount = 1;

    const GraphicsPipeline *pipeline = nullptr;
    const Descriptor *descriptors[16] = {nullptr};
    uint32_t descriptorCount = 0;

    std::byte pushContantData[128] = {(std::byte)0};
    size_t pushContantSize = 0;

    struct PipelineSettings
    {
        CullMode cullMode = CullMode::Back;
        bool enableDepthTest = true;
        bool enableDepthWrite = true;
    } pipelineSettings;

    std::string debugName = "Untitled";
};