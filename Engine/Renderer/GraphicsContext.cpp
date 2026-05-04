#include "GraphicsContext.hpp"
#include "Core/Macro.hpp"
#include "GLFW/glfw3.h"

VkBool32 validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, 
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    ERROR("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    ERROR("{}", pCallbackData->pMessage);
    ERROR("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

    return VK_FALSE;
};


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

        extensions.push_back("VK_EXT_debug_utils");


        uint32_t layerCount = 1;
        const char* layers[] = { "VK_LAYER_KHRONOS_validation" };

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = layerCount;
        createInfo.ppEnabledLayerNames = layers;

        vkCreateInstance(&createInfo, nullptr, &mInstance);
    }

    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = 
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT  | 
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT     |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT  |
                            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
            .pfnUserCallback = validationCallback,
            .pUserData = nullptr,
        };

        auto CreateDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");

        VkDebugUtilsMessengerEXT messenger;
        CreateDebugMessenger(mInstance, &createInfo, nullptr, &messenger);
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
    sCurrentContext = this;
}

GraphicsContext& GraphicsContext::GetCurrentContext() 
{
    return *sCurrentContext;
}

VkInstance GraphicsContext::GetInstance()
{
    return mInstance;
}
VkPhysicalDevice GraphicsContext::GetPhysicalDevice()
{
    return mPhysicalDevice;
}
VkDevice GraphicsContext::GetDevice()
{
    return mDevice;
}
VkSurfaceKHR GraphicsContext::GetSurface()
{
    return mSurface;
}
QueueIndices GraphicsContext::GetQueueIndices()
{
    return mQueueIndices;
}
Queues GraphicsContext::GetQueues()
{
    return mQueues;
}
VkCommandPool GraphicsContext::GetCommandPool()
{
    return mCommandPool;
}

VkDebugUtilsMessengerEXT GraphicsContext::GetMessenger() 
{
    return mMessenger;    
}

GraphicsContext* GraphicsContext::sCurrentContext = nullptr;


VkInstance getInstance()
{
    return GraphicsContext::GetCurrentContext().GetInstance();
}
VkPhysicalDevice getPhysicalDevice()
{
    return GraphicsContext::GetCurrentContext().GetPhysicalDevice();
}
VkDevice getDevice()
{
    return GraphicsContext::GetCurrentContext().GetDevice();
}
VkSurfaceKHR getSurface()
{
    return GraphicsContext::GetCurrentContext().GetSurface();
}
QueueIndices getQueueIndices()
{
    return GraphicsContext::GetCurrentContext().GetQueueIndices();
}
Queues getQueues()
{
    return GraphicsContext::GetCurrentContext().GetQueues();
}
VkCommandPool getCommandPool()
{
    return GraphicsContext::GetCurrentContext().GetCommandPool();
}

VkDebugUtilsMessengerEXT getMessenger() 
{
    return GraphicsContext::GetCurrentContext().GetMessenger();    
}
