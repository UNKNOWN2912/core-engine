#pragma once
#include "Core/Window.hpp"
#include "RenderPass.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/Swapchain.hpp"


struct GraphicData;


struct VertexBuffer
{

};

struct IndexBuffer
{

};

struct Viewport
{
    glm::uvec2 size;
    glm::uvec2 position;
};

enum class QueueType
{
    Graphic,
    Compute,
    Transfer, 
    Present
};

class Graphic
{
public:
    void Initialize(const Window& window);
    void Terminate();

    void BeginCommandBufferRecording(const CommandBuffer& commandBuffer);
    void EndCommandBufferRecording(const CommandBuffer& commandBuffer);

    void BeginRenderPass(const CommandBuffer& commandBuffer, const RenderPass& renderPass, const FrameBuffer& frameBuffer, const Viewport& viewport);
    void EndRenderPass(const CommandBuffer& commandBuffer);

    void BindVertexBuffer(const VertexBuffer& vertexBuffer, uint32_t bindIndex);
    void BindIndexBuffer(const IndexBuffer& indexBuffer);

    void DrawIndexed(uint32_t index);

    void CreateRenderPass(RenderPass& renderPass);
    void CreateCommandBuffer(CommandBuffer& commandBuffer);
    void CreateFrameBuffer(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport);
    void CreateFrameBufferWithoutAttachments(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport);
    void CreateDeviceImage(DeviceImage& image);
    void CreateSwapchain(Swapchain& swapchain);

    void ExecuteCommandBuffer(CommandBuffer& commandBuffer, QueueType queueType);

    void WaitForDevice();
private:
    void Clean();
    void DestroyVulkanObjects();
    void CreateVulkanObjects(const Window& window);
    GraphicData* mData = nullptr;
};









