#include "Renderer/CommandBuffer.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/Utility.hpp"


void CommandBuffer::Create(VkCommandPool commandPool)
{
    mHandle = AllocateCommandBuffer(commandPool);
    mCommandPool = commandPool;
}
void CommandBuffer::Destroy()
{
    vkFreeCommandBuffers(getDevice(), mCommandPool, 1, &mHandle);
}
void CommandBuffer::BeginRecording(bool oneTimeSubmit)
{
    VkCommandBufferUsageFlags usage = 0;

    if(oneTimeSubmit)
    {
        usage = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    }
    VkCommandBufferBeginInfo beginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = usage,
    };
    
    vkBeginCommandBuffer(mHandle, &beginInfo);
}
void CommandBuffer::EndRecording()
{
    vkEndCommandBuffer(mHandle);
}

void CommandBuffer::QueueSubmit(VkQueue queue, const Semaphore& waitSemaphore, const Semaphore& signalSemaphore, PipelineStage waitStage) 
{

    VkPipelineStageFlags vkWaitStage[] = {GetVulkanPipelineStage(waitStage)};
    uint32_t waitSemaphoreCount = (waitSemaphore.GetHandle() == VK_NULL_HANDLE) ? 0 : 1;
    uint32_t signalSemaphoreCount = (signalSemaphore.GetHandle() == VK_NULL_HANDLE) ? 0 : 1;

    VkSemaphore waitSemaphoreHandle = waitSemaphore.GetHandle();
    VkSemaphore signalSemaphoreHandle = signalSemaphore.GetHandle();

    VkSubmitInfo submitInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = waitSemaphoreCount,
        .pWaitSemaphores = &waitSemaphoreHandle,
        .pWaitDstStageMask = vkWaitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &mHandle,
        .signalSemaphoreCount = signalSemaphoreCount,
        .pSignalSemaphores = &signalSemaphoreHandle
    };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}
