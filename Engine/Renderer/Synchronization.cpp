#include "Synchronization.hpp"
#include "Renderer/GraphicsContext.hpp"

void Semaphore::Create() 
{
    VkSemaphoreCreateInfo createInfo =
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    vkCreateSemaphore(getDevice(), &createInfo, nullptr, &mHandle);
}

void Fence::Create(bool signaled) 
{
    VkFenceCreateFlags flag;
    if(signaled)
        flag = VK_FENCE_CREATE_SIGNALED_BIT;

    VkFenceCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .flags = flag
    };
    
    vkCreateFence(getDevice(), &createInfo, nullptr, &mHandle);
}

