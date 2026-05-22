#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Synchronization.hpp"
#include "Renderer/Types.hpp"

class CommandBuffer
{
public:
    void Create(VkCommandPool commandPool = GraphicsContext::GetCommandPool());
    void Destroy();

    void BeginRecording(bool oneTimeSubmit = false);
    void EndRecording();

    void QueueSubmit(VkQueue queue, const Semaphore &waitSemaphore = {}, const Semaphore &signalSemaphore = {}, PipelineStage waitStage = PipelineStage::TopOfPipe);

    VkCommandBuffer GetHandle() const
    {
        return mHandle;
    }

    ~CommandBuffer();

private:
    VkCommandBuffer mHandle = VK_NULL_HANDLE;
    VkCommandPool mCommandPool = VK_NULL_HANDLE;
};