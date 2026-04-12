#pragma once
#include <vulkan/vulkan.h>

class RenderPass
{
    public:
        void Create();
        void Destroy();

        VkRenderPass GetHandle() const { return mHandle; }
    private:
        VkRenderPass mHandle = VK_NULL_HANDLE;
        
};