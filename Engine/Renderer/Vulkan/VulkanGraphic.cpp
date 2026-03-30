#include <Renderer/Graphic.hpp>
#include "Renderer/Vulkan/VulkanSwapchain.hpp"
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

struct QueueIndex
{
	uint32_t graphic = UINT32_MAX;
	uint32_t transfer = UINT32_MAX;
	uint32_t present = UINT32_MAX;
	uint32_t compute = UINT32_MAX;
};

struct Queue
{
	VkQueue graphic = VK_NULL_HANDLE;
	VkQueue transfer = VK_NULL_HANDLE;
	VkQueue present = VK_NULL_HANDLE;
	VkQueue compute = VK_NULL_HANDLE;
};


struct GraphicData
{
    VkInstance instance = VK_NULL_HANDLE;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;
	VkCommandPool commandPool = VK_NULL_HANDLE;
	VulkanSwapchain swapchain;
	QueueIndex queueIndices;
	Queue queues;
	VkSurfaceKHR surface = VK_NULL_HANDLE;
};

struct VulkanHelper
{
    static VkInstance CreateInstance(bool enableValidation = false);
    static VkPhysicalDevice GetPhysicalDevice(VkInstance instance);
    static VkDevice CreateDevice(VkPhysicalDevice physicalDevice, const QueueIndex& queueIndices);
    static VkCommandPool CreateCommandPool(VkDevice device);
    static VulkanSwapchain CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const Window& window);
    static QueueIndex GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static Queue GetQueues(VkDevice device, const QueueIndex& queueIndices);
    static VkSurfaceKHR CreateSurface(VkInstance instance, const Window& window);
};


void Graphic::Initialize(const Window& window)
{
    if(mData != nullptr)
        Clean();
    
    mData = new GraphicData;
    CreateVulkanObjects(window);
}

void Graphic::Terminate() 
{
    Clean();
}

void Graphic::Clean() 
{
    DestroyVulkanObjects();
    delete mData;
}

void Graphic::CreateVulkanObjects(const Window& window) 
{
    mData->instance = VulkanHelper::CreateInstance();
    mData->physicalDevice = VulkanHelper::GetPhysicalDevice(mData->instance);
    mData->surface = VulkanHelper::CreateSurface(mData->instance, window);
    mData->queueIndices = VulkanHelper::GetQueueIndices(mData->physicalDevice, mData->surface);
    mData->device = VulkanHelper::CreateDevice(mData->physicalDevice, mData->queueIndices);
    mData->queues = VulkanHelper::GetQueues(mData->device, mData->queueIndices);
    mData->commandPool = VulkanHelper::CreateCommandPool(mData->device);
    mData->swapchain = VulkanHelper::CreateSwapchain(mData->physicalDevice, mData->device, mData->surface, window);
}

void Graphic::DestroyVulkanObjects() 
{
    mData->swapchain.Destroy();
    vkDestroySurfaceKHR(mData->instance, mData->surface, nullptr);
    vkDestroyDevice(mData->device, nullptr);
    vkDestroyCommandPool(mData->device, mData->commandPool, nullptr);
    vkDestroyInstance(mData->instance, nullptr);    
}



VkInstance VulkanHelper::CreateInstance(bool enableValidation)
{
	VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pApplicationName = "test";
	appInfo.apiVersion = VK_API_VERSION_1_4;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "test";

	VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

	createInfo.pApplicationInfo = &appInfo;

	uint32_t extensionCount = 0;
	const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensions;


        
	const char *layers = {"VK_LAYER_KHRONOS_validation"};

    if(enableValidation)
    {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = &layers;
    }

    VkInstance instance;
	vkCreateInstance(&createInfo, nullptr, &instance);
    return instance;
}

VkPhysicalDevice VulkanHelper::GetPhysicalDevice(VkInstance instance) 
{
	assert(instance != VK_NULL_HANDLE);
	
	uint32_t count;

    vkEnumeratePhysicalDevices(instance, &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkEnumeratePhysicalDevices(instance, &count, physicalDevices.data());

    VkPhysicalDeviceProperties properties;

    for (VkPhysicalDevice physicalDevice : physicalDevices)
    {
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            return physicalDevice;
        }
    }

    return VK_NULL_HANDLE;
    
}

VkDevice VulkanHelper::CreateDevice(VkPhysicalDevice physicalDevice, const QueueIndex& queueIndices)
{
	assert(physicalDevice != VK_NULL_HANDLE);

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    const char *extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = &extension;

    std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos;

    float priority = 1.f;

    VkDeviceQueueCreateInfo graphicQueueCreateInfo = {};
    graphicQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphicQueueCreateInfo.queueCount = 1;
    graphicQueueCreateInfo.pQueuePriorities = &priority;
    graphicQueueCreateInfo.queueFamilyIndex = queueIndices.graphic;

    VkDeviceQueueCreateInfo presentQueueCreateInfo = {};
    presentQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    presentQueueCreateInfo.queueCount = 1;
    presentQueueCreateInfo.pQueuePriorities = &priority;
    presentQueueCreateInfo.queueFamilyIndex = queueIndices.present;

    VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
    computeQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    computeQueueCreateInfo.queueCount = 1;
    computeQueueCreateInfo.pQueuePriorities = &priority;
    computeQueueCreateInfo.queueFamilyIndex = queueIndices.compute;

    VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
    transferQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    transferQueueCreateInfo.queueCount = 1;
    transferQueueCreateInfo.pQueuePriorities = &priority;
    transferQueueCreateInfo.queueFamilyIndex = queueIndices.transfer;

    queuesCreateInfos.push_back(graphicQueueCreateInfo);
    if (queueIndices.present != queueIndices.graphic && queueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(presentQueueCreateInfo);
    }

    if (queueIndices.compute != queueIndices.present && queueIndices.compute != queueIndices.graphic && queueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(computeQueueCreateInfo);
    }

    if (queueIndices.transfer != queueIndices.present && queueIndices.transfer != queueIndices.graphic && queueIndices.transfer != queueIndices.compute && queueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(transferQueueCreateInfo);
    }

    createInfo.pQueueCreateInfos = queuesCreateInfos.data();
    createInfo.queueCreateInfoCount = queuesCreateInfos.size();

    VkDevice device;
    vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
    return device;
}

QueueIndex VulkanHelper::GetQueueIndices(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
	assert(physicalDevice != VK_NULL_HANDLE);
	assert(surface != VK_NULL_HANDLE);

    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &count, properties.data());

    QueueIndex queueIndices;

    for (size_t i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueIndices.graphic == UINT32_MAX)
        {
            queueIndices.graphic = i;
        }
        if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueIndices.compute == UINT32_MAX)
        {
            queueIndices.transfer = i;
        }
        if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && queueIndices.compute == UINT32_MAX)
        {
            queueIndices.compute = i;
        }
        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported);
        if (supported == VK_TRUE && queueIndices.present == UINT32_MAX)
        {
            queueIndices.present = i;
        }
    }

    return queueIndices;
}

Queue VulkanHelper::GetQueues(VkDevice device, const QueueIndex& queueIndices)
{
    assert(device != VK_NULL_HANDLE);

    Queue queues;

    vkGetDeviceQueue(device, queueIndices.graphic, 0, &queues.graphic);
    vkGetDeviceQueue(device, queueIndices.present, 0, &queues.present);
    vkGetDeviceQueue(device, queueIndices.compute, 0, &queues.compute);
    vkGetDeviceQueue(device, queueIndices.transfer, 0, &queues.transfer);

    return queues;
}

VkSurfaceKHR VulkanHelper::CreateSurface(VkInstance instance, const Window& window) 
{
    assert(instance != VK_NULL_HANDLE);
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance, (GLFWwindow*)window.GetNativeWindow(), nullptr, &surface);
    return surface;
}

VkCommandPool VulkanHelper::CreateCommandPool(VkDevice device)
{
    assert(device != VK_NULL_HANDLE);

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    VkCommandPool commandPool;
    vkCreateCommandPool(device, &createInfo, nullptr, &commandPool);
    return commandPool;
}

VulkanSwapchain VulkanHelper::CreateSwapchain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, const Window& window) 
{
    assert(physicalDevice != VK_NULL_HANDLE);
    assert(device != VK_NULL_HANDLE);
    assert(surface != VK_NULL_HANDLE);
    VulkanSwapchain swapchain;
    swapchain.Create({window.GetSize().x, window.GetSize().y}, physicalDevice, device, surface);

    return swapchain;
}
