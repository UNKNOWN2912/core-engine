#pragma once
#include "Core/Window.hpp"
#include "RenderPass.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/DeviceSemaphore.hpp"
#include "Renderer/FrameBuffer.hpp"
#include "Renderer/GraphicPipeline.hpp"
#include "Renderer/PipelineLayout.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/Swapchain.hpp"


struct GraphicData;


struct VertexBuffer
{

};

struct IndexBuffer
{

};

class Viewport
{
    public:
        Viewport& SetSize(const glm::uvec2& size) { mSize = size; return *this; }
        Viewport& SetPosition(const glm::uvec2& position) { mPosition = position; return *this; }

        const glm::uvec2& GetSize() const { return mSize; }
        const glm::uvec2& GetPosition() const { return mPosition; }
    private:
        glm::uvec2 mSize;
        glm::uvec2 mPosition;
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
    void CreateFrameBufferWithUserAttachments(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport, const std::vector<DeviceImage>& images);
    void CreateDeviceImage(DeviceImage& image);
    void CreateSwapchain(Swapchain& swapchain);
    void CreateSemaphore(DeviceSemaphore& semaphore);
    void CreateShader(Shader& shader, const std::vector<char>& code);
    void CreateShaderFromFile(Shader& shader, std::string_view path);
    void CreatePipelineLayout(PipelineLayout& pipelineLayout);
    void CreateGraphicPipeline(GraphicPipeline& graphicPipeline, const RenderPass& renderPass, uint64_t subpassIndex, const Viewport& viewport);

    void ExecuteCommandBuffer(const CommandBuffer& commandBuffer, QueueType queueType, const std::vector<DeviceSemaphore>& waitSemaphores, DeviceSemaphore signalSemaphore);
    void WaitForDevice();
    void PresentSwapchainImage(const Swapchain& swapchain, uint32_t index, DeviceSemaphore waitSemaphore);

    uint32_t GetNextSwapchainImage(const Swapchain& swapchain, DeviceSemaphore semaphore);
private:
    void Clean();
    void DestroyVulkanObjects();
    void CreateVulkanObjects(const Window& window);
    GraphicData* mData = nullptr;
};






