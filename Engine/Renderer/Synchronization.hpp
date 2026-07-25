#pragma once
#include <vulkan/vulkan.h>

class Semaphore
{
public:
    void CreateSemaphore();
    void DestorySemaphore();
    VkSemaphore GetHandle() const;

private:
    VkSemaphore mHandle = VK_NULL_HANDLE;
};

class Fence
{
public:
    void CreateFence(bool signaled = false);
    void DestroyFence();
    VkFence GetHandle() const;

private:
    VkFence mHandle = VK_NULL_HANDLE;
};