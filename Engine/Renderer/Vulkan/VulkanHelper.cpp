#include "VulkanHelper.hpp"
#include "GLFW/glfw3.h"
#include <Core/Macro.hpp>

VkFormat VulkanHelper::ConvertToVulkanFormat(ImageFormat format) 
{
    std::pair<ImageFormat, VkFormat> formatMap[] = {
        { ImageFormat::None, VK_FORMAT_UNDEFINED },
        { ImageFormat::BGRA8, VK_FORMAT_B8G8R8A8_SRGB },
        { ImageFormat::R8, VK_FORMAT_R8_SRGB },
        { ImageFormat::RG8, VK_FORMAT_R8G8_SRGB },
        { ImageFormat::RGB8, VK_FORMAT_R8G8B8_SRGB },
        { ImageFormat::RGBA8, VK_FORMAT_R8G8B8A8_SRGB },
        { ImageFormat::R16, VK_FORMAT_R16_SFLOAT },
        { ImageFormat::RG16, VK_FORMAT_R16G16_SFLOAT },
        { ImageFormat::RGB16, VK_FORMAT_R16G16B16_SFLOAT },
        { ImageFormat::RGBA16, VK_FORMAT_R16G16B16A16_SFLOAT },
        { ImageFormat::R32, VK_FORMAT_R32_SFLOAT },
        { ImageFormat::RG32, VK_FORMAT_R32G32_SFLOAT },
        { ImageFormat::RGB32, VK_FORMAT_R32G32B32_SFLOAT },
        { ImageFormat::RGBA32, VK_FORMAT_R32G32B32A32_SFLOAT },
        { ImageFormat::R64, VK_FORMAT_R64_SFLOAT },
        { ImageFormat::RG64, VK_FORMAT_R64G64_SFLOAT },
        { ImageFormat::RGB64, VK_FORMAT_R64G64B64_SFLOAT },
        { ImageFormat::RGBA64, VK_FORMAT_R64G64B64A64_SFLOAT }
    };

    for(auto [imgFormat, vkFormat] : formatMap)
    {
        if(imgFormat == format)
            return vkFormat;
    }

    ERROR("Unknown Image Format: {}", (uint32_t)format);
    
    return VK_FORMAT_UNDEFINED;
}

VkImageLayout VulkanHelper::GetLayoutFromAttachmentUsage(ImageUsage usage) 
{

    std::pair<ImageUsage, VkImageLayout> layoutMap[] = 
    {
        {ImageUsage::None, VK_IMAGE_LAYOUT_UNDEFINED},
        {ImageUsage::ColorOutput, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL},
        {ImageUsage::SubpassInput, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {ImageUsage::Depth, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},
        {ImageUsage::Present, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR},
        {ImageUsage::Sample, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {ImageUsage::Storage, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
        {ImageUsage::TransferSrc, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL},
        {ImageUsage::TransferDst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL}
    };

    for(auto [imgUsage, vkLayout] : layoutMap)
    {
        if((usage & imgUsage) != ImageUsage::None)
            return vkLayout;
    }

    ERROR("Unknown Image Usage: {}", (uint32_t)usage);

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkAttachmentLoadOp VulkanHelper::ConvertToVulkanLoadOperation(LoadOperation loadOperation) 
{
    std::pair<LoadOperation, VkAttachmentLoadOp> loadOpMap[] = {
        {LoadOperation::Clear, VK_ATTACHMENT_LOAD_OP_CLEAR},
        {LoadOperation::Load, VK_ATTACHMENT_LOAD_OP_LOAD},
        {LoadOperation::DontCare, VK_ATTACHMENT_LOAD_OP_DONT_CARE}
    };

    for(auto [op, vkOp] : loadOpMap)
    {
        if(op == loadOperation)
            return vkOp;
    }

    ERROR("Unknown Load Operation Type: {}", (uint32_t)loadOperation);

    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp VulkanHelper::ConvertToVulkanStoreOperation(StoreOperation storeOperation) 
{
    std::pair<StoreOperation, VkAttachmentStoreOp> storeOpMap[] = {
        {StoreOperation::Store, VK_ATTACHMENT_STORE_OP_STORE},
        {StoreOperation::DontCare, VK_ATTACHMENT_STORE_OP_DONT_CARE}
    };

    for(auto [op, vkOp] : storeOpMap)
    {
        if(op == storeOperation)
            return vkOp;
    }

    ERROR("Unknown Store Operation Type: {}", (uint32_t)storeOperation);

    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkAttachmentDescription VulkanHelper::GetVulkanAttachmentFromAttachment(const Attachment& attachment) 
{
    VkAttachmentDescription description = {};
    description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    description.finalLayout = VulkanHelper::GetLayoutFromAttachmentUsage(attachment.GetImageUsage());
    description.format = VulkanHelper::ConvertToVulkanFormat(attachment.GetFormat());
    description.loadOp = VulkanHelper::ConvertToVulkanLoadOperation(attachment.GetLoadOperation());
    description.storeOp = VulkanHelper::ConvertToVulkanStoreOperation(attachment.GetStoreOperation());
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

VkShaderStageFlags VulkanHelper::ConvertToVulkanShaderStage(ShaderType shaderType) 
{
    std::pair<ShaderType, VkShaderStageFlags> shaderStageMap[] = {
        {ShaderType::Vertex, VK_SHADER_STAGE_VERTEX_BIT},
        {ShaderType::Fragment, VK_SHADER_STAGE_FRAGMENT_BIT},
        {ShaderType::Compute, VK_SHADER_STAGE_COMPUTE_BIT},
    };

    for(auto [type, vkStage] : shaderStageMap)
    {
        if(type == shaderType)
            return vkStage;
    }

    ERROR("Unknown Shader Type: {}", (uint32_t)shaderType);

    return 0;
}

VkDescriptorType VulkanHelper::ConvertToVulkanDescriptorType(DescriptorType descriptorType) 
{
    std::pair<DescriptorType, VkDescriptorType> descriptorTypeMap[] = {
        {DescriptorType::UniformBuffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER},
        {DescriptorType::StorageBuffer, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER},
        {DescriptorType::CombinedSampler, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}
    };

    for(auto [type, vkType] : descriptorTypeMap)
    {
        if(type == descriptorType)
            return vkType;
    }

    ERROR("Unknown Descriptor Type: {}", (uint32_t)descriptorType);

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;    
}

VkPrimitiveTopology VulkanHelper::ConvertToVulkanPrimitiveTopology(PrimitiveType primitiveType) 
{
    std::pair<PrimitiveType, VkPrimitiveTopology> primitiveTypeMap[] = {
        {PrimitiveType::Triangle, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST},
        {PrimitiveType::Line, VK_PRIMITIVE_TOPOLOGY_LINE_LIST},
        {PrimitiveType::Point, VK_PRIMITIVE_TOPOLOGY_POINT_LIST}
    };

    for(auto [type, vkType] : primitiveTypeMap)
    {
        if(type == primitiveType)
            return vkType;
    }

    ERROR("Unknown PrimitiveType: {}", (uint32_t)primitiveType);

    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkCullModeFlags VulkanHelper::ConvertToVulkanCullMode(CullMode cullMode) 
{
    const std::pair<CullMode, VkCullModeFlags> cullModeMap[] = 
    {
        {CullMode::None, VK_CULL_MODE_NONE},
        {CullMode::Front, VK_CULL_MODE_FRONT_BIT},
        {CullMode::Back, VK_CULL_MODE_BACK_BIT}
    };

    for(auto [mode, vkMode] : cullModeMap)
    {
        if(mode == cullMode)
            return vkMode;
    }

    ERROR("Unknown CullMode: {}", (uint32_t)cullMode);

    return VK_CULL_MODE_NONE;
}

VkFrontFace VulkanHelper::ConvertToVulkanFrontFace(FrontFace frontFace) 
{
    const std::pair<FrontFace, VkFrontFace> frontFaceMap[] = 
    {
        {FrontFace::Clockwise, VK_FRONT_FACE_CLOCKWISE},
        {FrontFace::CounterClockwise, VK_FRONT_FACE_COUNTER_CLOCKWISE}
    };

    for(auto [face, vkFace] : frontFaceMap)
    {
        if(face == frontFace)
            return vkFace;
    }

    ERROR("Unknown FrontFace: {}", (uint32_t)frontFace);

    return VK_FRONT_FACE_COUNTER_CLOCKWISE;
}

VkFormat VulkanHelper::ConvertToVulkanVertexFormat(VertexFormatType format) 
{
    const std::pair<VertexFormatType, VkFormat> vertexFormatMap[] = 
    {
        {VertexFormatType::Float, VK_FORMAT_R32_SFLOAT},
        {VertexFormatType::Vec2, VK_FORMAT_R32G32_SFLOAT},
        {VertexFormatType::Vec3, VK_FORMAT_R32G32B32_SFLOAT},
        {VertexFormatType::Vec4, VK_FORMAT_R32G32B32A32_SFLOAT},
        {VertexFormatType::Int, VK_FORMAT_R32_SINT},
        {VertexFormatType::IVec2, VK_FORMAT_R32G32_SINT},
        {VertexFormatType::IVec3, VK_FORMAT_R32G32B32_SINT},
        {VertexFormatType::IVec4, VK_FORMAT_R32G32B32A32_SINT},
        {VertexFormatType::UInt, VK_FORMAT_R32_UINT},
        {VertexFormatType::UVec2, VK_FORMAT_R32G32_UINT},
        {VertexFormatType::UVec3, VK_FORMAT_R32G32B32_UINT},
        {VertexFormatType::UVec4, VK_FORMAT_R32G32B32A32_UINT},
        {VertexFormatType::Double, VK_FORMAT_R64_SFLOAT},
        {VertexFormatType::DVec2, VK_FORMAT_R64G64_SFLOAT},
        {VertexFormatType::DVec3, VK_FORMAT_R64G64B64_SFLOAT},
        {VertexFormatType::DVec4, VK_FORMAT_R64G64B64A64_SFLOAT},
        {VertexFormatType::Mat2, VK_FORMAT_R32G32_SFLOAT},
        {VertexFormatType::Mat3, VK_FORMAT_R32G32B32_SFLOAT},
        {VertexFormatType::Mat4, VK_FORMAT_R32G32B32A32_SFLOAT}
    };

    for(auto [vertexFormat, vkFormat] : vertexFormatMap)
    {
        if(vertexFormat == format)
            return vkFormat;
    }

    ERROR("Unknown Vertex Format Type: {}", (uint32_t)format);

    return VK_FORMAT_UNDEFINED;    
}

VkSampleCountFlagBits VulkanHelper::ConvertToVulkanSampleCount(uint32_t sampleCount) 
{
    const std::pair<uint32_t, VkSampleCountFlagBits> sampleCountMap[] = 
    {
        {1, VK_SAMPLE_COUNT_1_BIT},
        {2, VK_SAMPLE_COUNT_2_BIT},
        {4, VK_SAMPLE_COUNT_4_BIT},
        {8, VK_SAMPLE_COUNT_8_BIT},
        {16, VK_SAMPLE_COUNT_16_BIT},
        {32, VK_SAMPLE_COUNT_32_BIT},
        {64, VK_SAMPLE_COUNT_64_BIT}
    };

    for(auto [count, vkCount] : sampleCountMap)
    {
        if(count == sampleCount)
            return vkCount;
    }

    ERROR("Unknown Sample Count: {}", sampleCount);

    return VK_SAMPLE_COUNT_1_BIT;    
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
