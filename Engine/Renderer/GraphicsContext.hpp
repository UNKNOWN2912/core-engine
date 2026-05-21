#pragma once
#include "Core/Window.hpp"
#include <vulkan/vulkan.h>

struct QueueIndices
{
    uint32_t graphics = UINT32_MAX, transfer = UINT32_MAX, compute = UINT32_MAX, present = UINT32_MAX; 
};

struct Queues
{
    VkQueue graphics, transfer, compute, present; 
};



class GraphicsContext
{
    public:
        void Create(const Window& window, bool setAsCurrentContext);
        void Destroy();

        void SetAsCurrentContext();

        static GraphicsContext& GetCurrentContext();

        VkInstance GetInstance();
        VkPhysicalDevice GetPhysicalDevice();
        VkDevice GetDevice();
        VkSurfaceKHR GetSurface();
        QueueIndices GetQueueIndices();
        Queues GetQueues();
        VkCommandPool GetCommandPool();
        VkDebugUtilsMessengerEXT GetMessenger();
    private:
        static GraphicsContext* sCurrentContext;
        VkInstance mInstance;
        VkPhysicalDevice mPhysicalDevice;
        VkDevice mDevice;
        VkSurfaceKHR mSurface;
        QueueIndices mQueueIndices;
        Queues mQueues;
        VkCommandPool mCommandPool;
        VkDebugUtilsMessengerEXT mMessenger;
};

VkInstance getInstance();
VkPhysicalDevice getPhysicalDevice();
VkDevice getDevice();
VkSurfaceKHR getSurface();
QueueIndices getQueueIndices();
Queues getQueues();
VkCommandPool getCommandPool();
VkDebugUtilsMessengerEXT getMessenger();
