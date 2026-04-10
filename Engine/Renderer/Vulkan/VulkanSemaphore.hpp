#pragma once
#include <vulkan/vulkan.h>

class VulkanSemaphore 
{
public:
	VulkanSemaphore() : mData(Graphics::GetDataRef()) {}

	void Create() {}
	void Destroy() {}
	void HostWait() {}
private:
	GraphicsData& mData;
	VkSemaphore mHandle;
};
