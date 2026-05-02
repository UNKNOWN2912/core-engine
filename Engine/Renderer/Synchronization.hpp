#pragma once
#include <vulkan/vulkan.h>

class Semaphore
{
    public:
        void Create();
        VkSemaphore GetHandle() const { return mHandle; }

    private:
        VkSemaphore mHandle = VK_NULL_HANDLE;
};

class Fence
{
    public:
        void Create(bool signaled = false);
        VkFence GetHandle() const { return mHandle; }

    private:
        VkFence mHandle = VK_NULL_HANDLE;
};