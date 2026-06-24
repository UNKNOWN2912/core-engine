#pragma once
#include "Core/Window.hpp"
#include "Renderer/Types.hpp"
#include <vulkan/vulkan.h>

struct QueueIndices
{
    uint32_t graphics = UINT32_MAX, transfer = UINT32_MAX, compute = UINT32_MAX;
};

struct Queues
{
    VkQueue graphics, transfer, compute;
};

struct GraphicsLimits
{
    struct Device
    {
        uint32_t memoryCapacity = 0;
        bool bindlessSupported = false;
    } device;
};

class GraphicsContext
{
public:
    static void Initialize(DeviceType deviceType);
    static void Terminate();

    static VkInstance GetInstance();
    static VkPhysicalDevice GetPhysicalDevice();
    static VkDevice GetDevice();
    static QueueIndices GetQueueIndices();
    static Queues GetQueues();
    static VkCommandPool GetCommandPool();
    static VkDebugUtilsMessengerEXT GetMessenger();
    static const GraphicsLimits &GetLimits();

private:
    static VkInstance mInstance;
    static VkPhysicalDevice mPhysicalDevice;
    static VkDevice mDevice;
    static QueueIndices mQueueIndices;
    static Queues mQueues;
    static VkCommandPool mCommandPool;
    static VkDebugUtilsMessengerEXT mMessenger;
    static GraphicsLimits mLimits;
};
