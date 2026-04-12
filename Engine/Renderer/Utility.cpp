#include "Utility.hpp"
#include "Core/Macro.hpp"
#include "GraphicsContext.hpp"

uint32_t FindMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags memoryProperties)
{
    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(getPhysicalDevice(), &properties);

    for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
    {
        bool isTypeBit = (typeBits & (1 << i)) != 0;
        bool isMemoryProperty = (memoryProperties & properties.memoryTypes[i].propertyFlags) == memoryProperties;

        if(isTypeBit && isMemoryProperty)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) 
{
    Buffer buffer;

    VkBufferCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    };

    vkCreateBuffer(getDevice(), &createInfo, nullptr, &buffer.handle);

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(getDevice(), buffer.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, memoryProperties),
    };

    vkAllocateMemory(getDevice(), &allocateInfo, nullptr, &buffer.memory);

    buffer.size = size;

    vkBindBufferMemory(getDevice(), buffer.handle, buffer.memory, 0);

    if((memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        vkMapMemory(getDevice(), buffer.memory, 0, requirements.size, 0, &buffer.map);
    }

    return buffer;
}

void DestroyBuffer(Buffer& buffer) 
{
    if(buffer.handle == VK_NULL_HANDLE)
        return;
    vkDestroyBuffer(getDevice(), buffer.handle, nullptr);
    vkFreeMemory(getDevice(), buffer.memory, nullptr);
    buffer = {};
}

void TransferBufferData(const Buffer& srcBuffer, Buffer& dstBuffer) 
{
    VkCommandBuffer commandBuffer = AllocateCommandBuffer();
    BeginCommandBuffer(commandBuffer, true);

    VkBufferCopy region = 
    {
        .srcOffset = 0,
        .dstOffset = 0,
        .size = srcBuffer.size,
    };

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &region);

    EndCommandBuffer(commandBuffer);
    ExecuteCommandBuffer(commandBuffer, getQueues().transfer);

    vkQueueWaitIdle(getQueues().transfer);

    vkFreeCommandBuffers(getDevice(), getCommandPool(), 1, &commandBuffer);
}


VkCommandBuffer AllocateCommandBuffer() 
{
        VkCommandBufferAllocateInfo allocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };    
    
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(getDevice(), &allocateInfo, &commandBuffer);
    return commandBuffer;
}

void BeginCommandBuffer(VkCommandBuffer commandBuffer, bool singleUse) 
{
    VkCommandBufferBeginInfo beginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .flags = (singleUse) ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : (VkCommandBufferUsageFlagBits)0,
    };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void EndCommandBuffer(VkCommandBuffer commandBuffer) 
{
    vkEndCommandBuffer(commandBuffer);    
}

void ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
{
    VkSubmitInfo submitInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .commandBufferCount = 1,
        .pCommandBuffers = &commandBuffer,
    };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}


VkShaderModule CreateShaderModuleFromMemory(VkDevice device, const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo = 
    { 
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .codeSize = code.size(),
        .pCode = (uint32_t*)code.data(),
    };

    VkShaderModule shaderModule;
    vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    return shaderModule;
}

VkShaderModule CreateShaderFromFile(VkDevice device, const char *filename)
{
    std::vector<char> code;
    FILE *fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
        ERROR("Failed to find file: {}", filename);
    }

    fseek(fp, 0L, SEEK_END);
    int64_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    code.resize(size);
    fread(code.data(), 1, size, fp);
    fclose(fp);

    return CreateShaderModuleFromMemory(device, code);
}