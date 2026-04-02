#include "VulkanHelper.hpp"
#include "GLFW/glfw3.h"

VkFormat VulkanHelper::ConvertToVulkanFormat(ImageFormat format) 
{
    switch (format) 
    {
        case ImageFormat::None:
            return VK_FORMAT_UNDEFINED;
        case ImageFormat::BGRA8:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case ImageFormat::R8:
            return VK_FORMAT_R8_SRGB;
        case ImageFormat::RG8:
            return VK_FORMAT_R8G8_SRGB;
        case ImageFormat::RGB8:
            return VK_FORMAT_R8G8B8_SRGB;
        case ImageFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::R16:
            return VK_FORMAT_R16_SFLOAT;
        case ImageFormat::RG16:
            return VK_FORMAT_R16G16_SFLOAT;
        case ImageFormat::RGB16:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case ImageFormat::RGBA16:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::R32:
            return VK_FORMAT_R32_SFLOAT;
        case ImageFormat::RG32:
            return VK_FORMAT_R32G32_SFLOAT;
        case ImageFormat::RGB32:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case ImageFormat::RGBA32:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ImageFormat::R64:
            return VK_FORMAT_R64_SFLOAT;
        case ImageFormat::RG64:
            return VK_FORMAT_R64G64_SFLOAT;
        case ImageFormat::RGB64:
            return VK_FORMAT_R64G64B64_SFLOAT;
        case ImageFormat::RGBA64:
            return VK_FORMAT_R64G64B64A64_SFLOAT;
        break;
    }
    return VK_FORMAT_UNDEFINED;
}

VkImageLayout VulkanHelper::GetLayoutFromAttachmentUsage(ImageUsage usage) 
{
    switch (usage) 
    {
        case ImageUsage::None:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageUsage::ColorOutput:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageUsage::SubpassInput:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageUsage::Depth:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageUsage::Present:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        case ImageUsage::Sample:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageUsage::Storage:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageUsage::TransferSrc:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageUsage::TransferDst:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
          break;
    }

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkAttachmentLoadOp VulkanHelper::ConvertToVulkanLoadOperation(LoadOperation loadOperation) 
{
    switch (loadOperation) 
    {
        case LoadOperation::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR; 
        case LoadOperation::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD; 
        case LoadOperation::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE; 
        break;
    }
}

VkAttachmentStoreOp VulkanHelper::ConvertToVulkanStoreOperation(StoreOperation storeOperation) 
{
    switch (storeOperation) 
    {
        case StoreOperation::Store:
            return VK_ATTACHMENT_STORE_OP_STORE; 
        case StoreOperation::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE; 
        break;
    }
}

VkAttachmentDescription VulkanHelper::GetVulkanAttachmentFromAttachment(const Attachment& attachment) 
{
        VkAttachmentDescription description = {};
        description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        description.finalLayout = VulkanHelper::GetLayoutFromAttachmentUsage(attachment.usage);
        description.format = VulkanHelper::ConvertToVulkanFormat(attachment.format);
        description.loadOp = VulkanHelper::ConvertToVulkanLoadOperation(attachment.loadOperation);
        description.storeOp = VulkanHelper::ConvertToVulkanStoreOperation(attachment.storeOperation);
        description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        description.samples = VK_SAMPLE_COUNT_1_BIT;

        return description;
}
uint32_t VulkanHelper::GetMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperty) 
{
    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &properties);

    for (size_t i = 0; i < properties.memoryTypeCount; i++) 
    {
        if (memoryTypeBits & (1 << i) && (properties.memoryTypes[i].propertyFlags & memoryProperty) == memoryProperty) 
        {
            return i;
        }
    }
    return UINT32_MAX;
}
VkDeviceMemory VulkanHelper::AllocateMemoryForImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImage image, VkMemoryPropertyFlags memoryProperty) 
{
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(device, image, &requirements);

    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = GetMemoryTypeIndex(physicalDevice, requirements.memoryTypeBits, memoryProperty);

    VkDeviceMemory memory;
    vkAllocateMemory(device, &allocateInfo, nullptr, &memory);
    vkBindImageMemory(device, image, memory, 0);
    return memory;
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
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
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
