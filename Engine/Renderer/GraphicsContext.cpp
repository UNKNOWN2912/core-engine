#include "GraphicsContext.hpp"
#include "Core/Macro.hpp"
#include "GLFW/glfw3.h"

void GraphicsContext::Create(const Window& window, bool setAsCurrentContext) 
{
    CHROME_TRACE_FUNCTION();
    {
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        uint32_t glfwExtensionCount;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions;

        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            extensions.push_back(glfwExtensions[i]);
        }


        uint32_t layerCount = 1;
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = layerCount;
        createInfo.ppEnabledLayerNames = layers;

        vkCreateInstance(&createInfo, nullptr, &mInstance);
    }

    {
        uint32_t count;
        vkEnumeratePhysicalDevices(mInstance, &count, nullptr);
        VkPhysicalDevice devices[8];
        vkEnumeratePhysicalDevices(mInstance, &count, devices);

        for (uint32_t i = 0; i < count; i++)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);

            if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            {
                mPhysicalDevice = devices[i];
            }
        }
        if(mPhysicalDevice == VK_NULL_HANDLE)
            ERROR("Failed to find suitable device");
    }

    {
        glfwCreateWindowSurface(mInstance, (GLFWwindow*)window.GetNativeWindow(), nullptr, &mSurface);
    }

    {
        uint32_t count;
    	vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties properties[8];
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, properties);

        QueueIndices queueIndices;

        for (uint32_t i = 0; i < count; i++)
        {
            if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueIndices.graphics == UINT32_MAX)
            {
                queueIndices.graphics = i;
            }
            if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueIndices.transfer == UINT32_MAX)
            {
                queueIndices.transfer = i;
            }
            if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && queueIndices.compute == UINT32_MAX)
            {
                queueIndices.compute = i;
            }

            VkBool32 supported = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &supported);
            if (supported && queueIndices.present == UINT32_MAX)
            {
                queueIndices.present = i;
            }

            mQueueIndices = queueIndices;
        }
    }

    {
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;


        std::vector<const char*> extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos;

        float priority = 1.f;

        VkDeviceQueueCreateInfo graphicQueueCreateInfo = {};
        graphicQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicQueueCreateInfo.queueCount = 1;
        graphicQueueCreateInfo.pQueuePriorities = &priority;
        graphicQueueCreateInfo.queueFamilyIndex = mQueueIndices.graphics;

        VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
        presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        presentQueueCreateInfo.queueCount = 1;
        presentQueueCreateInfo.pQueuePriorities = &priority;
        presentQueueCreateInfo.queueFamilyIndex = mQueueIndices.present;

        VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
        computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        computeQueueCreateInfo.queueCount = 1;
        computeQueueCreateInfo.pQueuePriorities = &priority;
        computeQueueCreateInfo.queueFamilyIndex = mQueueIndices.compute;

        VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
        transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        transferQueueCreateInfo.queueCount = 1;
        transferQueueCreateInfo.pQueuePriorities = &priority;
        transferQueueCreateInfo.queueFamilyIndex = mQueueIndices.transfer;

        queuesCreateInfos.push_back(graphicQueueCreateInfo);
        if (mQueueIndices.present != mQueueIndices.graphics && mQueueIndices.present != UINT32_MAX)
        {
            queuesCreateInfos.push_back(presentQueueCreateInfo);
        }

        if (mQueueIndices.compute != mQueueIndices.present && mQueueIndices.compute != mQueueIndices.graphics && mQueueIndices.present != UINT32_MAX)
        {
            queuesCreateInfos.push_back(computeQueueCreateInfo);
        }

        if (mQueueIndices.transfer != mQueueIndices.present && mQueueIndices.transfer != mQueueIndices.graphics && mQueueIndices.transfer != mQueueIndices.compute && mQueueIndices.present != UINT32_MAX)
        {
            queuesCreateInfos.push_back(transferQueueCreateInfo);
        }

        createInfo.pQueueCreateInfos = queuesCreateInfos.data();
        createInfo.queueCreateInfoCount = queuesCreateInfos.size();

        vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
    }

    {
        vkGetDeviceQueue(mDevice, mQueueIndices.graphics, 0, &mQueues.graphics);
        vkGetDeviceQueue(mDevice, mQueueIndices.present, 0, &mQueues.present);
        vkGetDeviceQueue(mDevice, mQueueIndices.compute, 0, &mQueues.compute);
        vkGetDeviceQueue(mDevice, mQueueIndices.transfer, 0, &mQueues.transfer);
    }


    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(mDevice, &createInfo, nullptr, &mCommandPool);
    }

    if(setAsCurrentContext)
        SetAsCurrentContext();
}

void GraphicsContext::Destroy() 
{
    CHROME_TRACE_FUNCTION();
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroyDevice(mDevice, nullptr);
	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);

    if(sCurrentContext == this)
    {
        sCurrentContext = nullptr;
    }
}

void GraphicsContext::SetAsCurrentContext() 
{
    CHROME_TRACE_FUNCTION();
    sCurrentContext = this;
}

GraphicsContext& GraphicsContext::GetCurrentContext() 
{
    CHROME_TRACE_FUNCTION();
    return *sCurrentContext;
}

VkInstance GraphicsContext::GetInstance()
{
    CHROME_TRACE_FUNCTION();
    return mInstance;
}
VkPhysicalDevice GraphicsContext::GetPhysicalDevice()
{
    CHROME_TRACE_FUNCTION();
    return mPhysicalDevice;
}
VkDevice GraphicsContext::GetDevice()
{
    CHROME_TRACE_FUNCTION();
    return mDevice;
}
VkSurfaceKHR GraphicsContext::GetSurface()
{
    CHROME_TRACE_FUNCTION();
    return mSurface;
}
QueueIndices GraphicsContext::GetQueueIndices()
{
    CHROME_TRACE_FUNCTION();
    return mQueueIndices;
}
Queues GraphicsContext::GetQueues()
{
    CHROME_TRACE_FUNCTION();
    return mQueues;
}
VkCommandPool GraphicsContext::GetCommandPool()
{
    CHROME_TRACE_FUNCTION();
    return mCommandPool;
}

GraphicsContext* GraphicsContext::sCurrentContext = nullptr;


VkInstance getInstance()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetInstance();
}
VkPhysicalDevice getPhysicalDevice()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetPhysicalDevice();
}
VkDevice getDevice()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetDevice();
}
VkSurfaceKHR getSurface()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetSurface();
}
QueueIndices getQueueIndices()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetQueueIndices();
}
Queues getQueues()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetQueues();
}
VkCommandPool getCommandPool()
{
    CHROME_TRACE_FUNCTION();
    return GraphicsContext::GetCurrentContext().GetCommandPool();
}
