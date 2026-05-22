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
    static void Initialize(const Window &window);
    static void Terminate();

    static VkInstance GetInstance();
    static VkPhysicalDevice GetPhysicalDevice();
    static VkDevice GetDevice();
    static VkSurfaceKHR GetSurface();
    static QueueIndices GetQueueIndices();
    static Queues GetQueues();
    static VkCommandPool GetCommandPool();
    static VkDebugUtilsMessengerEXT GetMessenger();

private:
    static VkInstance mInstance;
    static VkPhysicalDevice mPhysicalDevice;
    static VkDevice mDevice;
    static VkSurfaceKHR mSurface;
    static QueueIndices mQueueIndices;
    static Queues mQueues;
    static VkCommandPool mCommandPool;
    static VkDebugUtilsMessengerEXT mMessenger;
};