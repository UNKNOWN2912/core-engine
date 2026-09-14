#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Synchronization.hpp"
#include "Renderer/Types.hpp"

class CommandBuffer
{
public:
    void BeginRecording(bool oneTimeSubmit = false);
    void EndRecording();

    void QueueSubmit(VkQueue queue, const Semaphore &waitSemaphore = {}, const Semaphore &signalSemaphore = {}, PipelineStage waitStage = PipelineStage::TopOfPipe);

    VkCommandBuffer GetHandle() const;
    CommandBuffer();
    CommandBuffer(VkCommandPool commandPool);
    CommandBuffer(CommandBuffer &&commandBuffer) noexcept;
    CommandBuffer &operator=(CommandBuffer &&commandBuffer) noexcept;
    CommandBuffer(const CommandBuffer &) = delete;
    CommandBuffer &operator=(const CommandBuffer &) = delete;
    ~CommandBuffer();

private:
    void Create(VkCommandPool commandPool = GraphicsContext::GetCurrentContext().GetCommandPool());
    void Destroy();

    VkCommandBuffer mHandle = VK_NULL_HANDLE;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
};