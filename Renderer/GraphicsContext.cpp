#include "GraphicsContext.hpp"
#include "Core/Macro.hpp"
#include "Renderer/Converter.hpp"

VkBool32 validationCallback(VkDebugUtilsMessageSeverityFlagBitsEXT /*messageSeverity*/, VkDebugUtilsMessageTypeFlagsEXT /*messageTypes*/,
                            const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void * /*pUserData*/)
{
    ERROR("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");
    ERROR("{}", pCallbackData->pMessage);
    ERROR("---------------------------------------------------------------------------------------------------------------------------------------------------------------------------");

    return VK_FALSE;
};

void GraphicsContext::Initialize(DeviceType deviceType)
{
    CHROME_TRACE_FUNCTION();

    bool validationEnabled = false;

    {
        VkApplicationInfo appInfo =
            {
                .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                .pApplicationName = "RendererRework",
                .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                .pEngineName = "unknown-engine",
                .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                .apiVersion = VK_API_VERSION_1_4,
            };

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *> extensions;
        extensions.reserve(glfwExtensionCount);
        for (uint32_t i = 0; i < glfwExtensionCount; i++)
        {
            extensions.push_back(glfwExtensions[i]);
        }

        std::vector<const char *> layers;

        if (validationEnabled)
        {
            extensions.push_back("VK_EXT_debug_utils");
            layers.push_back("VK_LAYER_KHRONOS_validation");
        }

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = layers.size();
        createInfo.ppEnabledLayerNames = layers.data();

        vkCreateInstance(&createInfo, nullptr, &mInstance);
    }

    if (validationEnabled)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo =
            {
                .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
                .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                   VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
                .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
                .pfnUserCallback = validationCallback,
                .pUserData = nullptr,
            };

        auto createDebugMessenger = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT");

        VkDebugUtilsMessengerEXT messenger = {};
        createDebugMessenger(mInstance, &createInfo, nullptr, &messenger);
    }

    {
        uint32_t count = 0;
        vkEnumeratePhysicalDevices(mInstance, &count, nullptr);
        std::vector<VkPhysicalDevice> devices(count);
        vkEnumeratePhysicalDevices(mInstance, &count, devices.data());

        for (uint32_t i = 0; i < count; i++)
        {
            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(devices[i], &properties);

            if (properties.deviceType == GetVulkanDeviceType(deviceType))
            {
                mPhysicalDevice = devices[i];
            }
        }
        if (mPhysicalDevice == VK_NULL_HANDLE)
        {
            ERROR("Failed to find suitable device. Choosing available device");
            mPhysicalDevice = devices[0];
        }
    }

    {
        uint32_t count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, nullptr);
        VkQueueFamilyProperties properties[8];
        vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, (VkQueueFamilyProperties *)properties);

        QueueIndices queueIndices;

        for (uint32_t i = 0; i < count; i++)
        {
            if (((properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0u) && (queueIndices.graphics == UINT32_MAX))
            {
                queueIndices.graphics = i;
            }
            if (((properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0u) && (queueIndices.transfer == UINT32_MAX))
            {
                queueIndices.transfer = i;
            }
            if (((properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0u) && (queueIndices.compute == UINT32_MAX))
            {
                queueIndices.compute = i;
            }

            mQueueIndices = queueIndices;
        }
    }

    {

        VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures =
            {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
            };

        VkPhysicalDeviceFeatures2 features =
            {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                .pNext = &indexingFeatures,
            };

        vkGetPhysicalDeviceFeatures2(mPhysicalDevice, &features);

        mLimits.device.bindlessSupported = indexingFeatures.descriptorBindingPartiallyBound && indexingFeatures.runtimeDescriptorArray;

        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pNext = &features;

        std::vector<const char *> extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

        createInfo.enabledExtensionCount = extensions.size();
        createInfo.ppEnabledExtensionNames = extensions.data();

        std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos;

        float priority = 1.f;

        VkDeviceQueueCreateInfo graphicQueueCreateInfo = {};
        graphicQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        graphicQueueCreateInfo.queueCount = 1;
        graphicQueueCreateInfo.pQueuePriorities = &priority;
        graphicQueueCreateInfo.queueFamilyIndex = mQueueIndices.graphics;

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
        if (mQueueIndices.compute != mQueueIndices.graphics)
        {
            queuesCreateInfos.push_back(computeQueueCreateInfo);
        }
        if (mQueueIndices.transfer != mQueueIndices.graphics && mQueueIndices.transfer != mQueueIndices.compute)
        {
            queuesCreateInfos.push_back(transferQueueCreateInfo);
        }

        createInfo.pQueueCreateInfos = queuesCreateInfos.data();
        createInfo.queueCreateInfoCount = queuesCreateInfos.size();

        vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
    }

    {
        vkGetDeviceQueue(mDevice, mQueueIndices.graphics, 0, &mQueues.graphics);
        vkGetDeviceQueue(mDevice, mQueueIndices.compute, 0, &mQueues.compute);
        vkGetDeviceQueue(mDevice, mQueueIndices.transfer, 0, &mQueues.transfer);
    }

    {
        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        vkCreateCommandPool(mDevice, &createInfo, nullptr, &mCommandPool);
    }
}

void GraphicsContext::Terminate()
{
    CHROME_TRACE_FUNCTION();
    auto destroyDebugMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT");
    destroyDebugMessenger(mInstance, mMessenger, nullptr);
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroyDevice(mDevice, nullptr);
    vkDestroyInstance(mInstance, nullptr);

    mDevice = VK_NULL_HANDLE;
    mInstance = VK_NULL_HANDLE;
    mCommandPool = VK_NULL_HANDLE;
    mMessenger = VK_NULL_HANDLE;
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

VkInstance GraphicsContext::mInstance = VK_NULL_HANDLE;
VkPhysicalDevice GraphicsContext::mPhysicalDevice = VK_NULL_HANDLE;
VkDevice GraphicsContext::mDevice = VK_NULL_HANDLE;
VkCommandPool GraphicsContext::mCommandPool = VK_NULL_HANDLE;
VkDebugUtilsMessengerEXT GraphicsContext::mMessenger = VK_NULL_HANDLE;
QueueIndices GraphicsContext::mQueueIndices;
Queues GraphicsContext::mQueues;
GraphicsLimits GraphicsContext::mLimits;