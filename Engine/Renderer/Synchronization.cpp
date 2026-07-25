#include "Synchronization.hpp"
#include "Renderer/GraphicsContext.hpp"

void Semaphore::CreateSemaphore()
{
    VkSemaphoreCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

    vkCreateSemaphore(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void Semaphore::DestorySemaphore()
{
    vkDestroySemaphore(GraphicsContext::GetDevice(), mHandle, nullptr);
}
VkSemaphore Semaphore::GetHandle() const
{
    return mHandle;
}

void Fence::CreateFence(bool signaled)
{
    VkFenceCreateFlags flag = 0;
    if (signaled)
    {
        flag = VK_FENCE_CREATE_SIGNALED_BIT;
    }

    VkFenceCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = flag};

    vkCreateFence(GraphicsContext::GetDevice(), &createInfo, nullptr, &mHandle);
}

void Fence::DestroyFence()
{
    vkDestroyFence(GraphicsContext::GetDevice(), mHandle, nullptr);
}
VkFence Fence::GetHandle() const
{
    return mHandle;
}
