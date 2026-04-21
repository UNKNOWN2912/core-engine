#pragma once
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Types.hpp"

class CommandBuffer
{
    public:
        void Create(VkCommandPool commandPool = getCommandPool());
        void Destroy();
        
        void BeginRecording(bool oneTimeSubmit = false);
        void EndRecording();

        void QueueSubmit(VkQueue queue, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, PipelineStage waitStage);

        VkCommandBuffer GetHandle() const { return mHandle; }
    private:
        VkCommandBuffer mHandle = VK_NULL_HANDLE;
        VkCommandPool mCommandPool = VK_NULL_HANDLE;
};