#include "VulkanGraphics.hpp"
#include "VulkanHelper.hpp"

GraphicsData Graphics::sData;

void Graphics::Initialize(const Window& window, DeviceType deviceType)
{
	sData.instance = VulkanHelper::CreateInstance();
	sData.physicalDevice = VulkanHelper::GetPhysicalDevice(sData.instance, VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
	sData.surface = VulkanHelper::CreateSurface(sData.instance, window);
	sData.queueIndices = VulkanHelper::GetQueueIndices(sData.physicalDevice, sData.surface);
	sData.device = VulkanHelper::CreateDevice(sData.instance, sData.physicalDevice, sData.queueIndices);
	sData.queues = VulkanHelper::GetQueues(sData.device, sData.queueIndices);
	sData.commandPool = VulkanHelper::CreateCommandPool(sData.device);
}

void Graphics::Terminate()
{
	vkDestroyDevice(sData.device, nullptr);
	vkDestroyCommandPool(sData.device, sData.commandPool, nullptr);
	vkDestroySurfaceKHR(sData.instance, sData.surface, nullptr);
	vkDestroyInstance(sData.instance, nullptr);
}

GraphicsData Graphics::GetGraphicsData()
{
	return sData;
}
