#include "VulkanSemaphore.hpp"
#include "VulkanHelper.hpp"
#include <vulkan/vulkan.h>
#include <Renderer/Graphics.hpp>


void VulkanSemaphore::Create()
{
	mHandle = VulkanHelper::CreateSemaphore(Graphics::GetData().device);
}

void VulkanSemaphore::Destroy()
{
	vkDestroySemaphore(mData.device, mHandle, nullptr);
}

void VulkanSemaphore::HostWait()
{
	VkSemaphoreWaitInfo waitInfo = {};
	waitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	waitInfo.semaphoreCount = 1;
	waitInfo.pSemaphores = &mHandle;

	vkWaitSemaphores(mData.device, &waitInfo, UINT64_MAX);
}
