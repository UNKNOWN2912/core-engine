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
    void Create(DeviceType deviceType);
    void Destroy();

    VkInstance GetInstance() const;
    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetDevice() const;
    QueueIndices GetQueueIndices() const;
    Queues GetQueues() const;
    VkCommandPool GetCommandPool() const;
    VkDebugUtilsMessengerEXT GetMessenger() const;
    const GraphicsLimits &GetLimits();

    static const GraphicsContext &GetCurrentContext();

    void SetAsCurrentContext();

private:
    static GraphicsContext *mCurrentContext;

    VkInstance mInstance;
    VkPhysicalDevice mPhysicalDevice;
    VkDevice mDevice;
    QueueIndices mQueueIndices;
    Queues mQueues;
    VkCommandPool mCommandPool;
    VkDebugUtilsMessengerEXT mMessenger;
    GraphicsLimits mLimits;
};
