#pragma once
#include "Core/Window.hpp"
#include "RenderPass.hpp"


struct GraphicData;


struct VertexBuffer
{

};

struct IndexBuffer
{

};

struct CommandBuffer
{

};

class Graphic
{
public:
    void Initialize(const Window& window);
    void Terminate();

    void BeginCommandBufferRecording(const CommandBuffer& commandBuffer);
    void EndCommandBufferRecording(const CommandBuffer& commandBuffer);

    void BeginRenderPass(const RenderPass& renderPass);
    void EndRenderPass();

    void BindVertexBuffer(const VertexBuffer& vertexBuffer, uint32_t bindIndex);
    void BindIndexBuffer(const IndexBuffer& indexBuffer);

    void DrawIndexed(uint32_t index);

private:
    void Clean();
    void DestroyVulkanObjects();
    void CreateVulkanObjects(const Window& window);
    GraphicData* mData = nullptr;
};




