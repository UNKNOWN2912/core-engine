#include "VulkanCommandBuffer.hpp"

void VulkanCommandBuffer::Create()
{
	VulkanHelper::AllocateCommandBuffer(Graphics::GetData().device, Graphics::GetData().commandPool);
}

void VulkanCommandBuffer::Reset()
{
	vkResetCommandBuffer(VulkanHelper::GetIdData<CommandBufferData>(mId).handle, 0);
}

void VulkanCommandBuffer::BeginRecording()
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	vkBeginCommandBuffer(VulkanHelper::GetIdData<CommandBufferData>(mId).handle, &beginInfo);
}

void VulkanCommandBuffer::EndRecording()
{
	vkEndCommandBuffer(VulkanHelper::GetIdData<CommandBufferData>(mId).handle);
}

void VulkanCommandBuffer::Execute(QueueType queueType, Semaphore waitSemaphore, Semaphore signalSemaphore, PipelineStage stages)
{
	VkSemaphore signalSemaphoreHandle = VulkanHelper::GetIdData<SemaphoreData>(signalSemaphore.GetId()).handle;
	VkSemaphore waitSemaphoreHandle = VulkanHelper::GetIdData<SemaphoreData>(signalSemaphore.GetId()).handle;
	VkPipelineStageFlags waitStage = VulkanConverter::GetPipelineStage(stages);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &VulkanHelper::GetIdData<CommandBufferData>(mId).handle;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &signalSemaphoreHandle;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &waitSemaphoreHandle;
}
