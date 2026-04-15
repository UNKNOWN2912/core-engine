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

void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, Image& image)
{
    VkCommandBuffer commandBuffer = AllocateCommandBuffer();
    BeginCommandBuffer(commandBuffer, true);

    VkImageMemoryBarrier barrier = 
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .image = image.handle,
        .subresourceRange = 
        {
            .aspectMask = aspectMask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndCommandBuffer(commandBuffer);
    ExecuteCommandBuffer(commandBuffer, getQueues().transfer);
}

void TransferImageData(const Buffer& srcBuffer, Image& dstImage, VkImageAspectFlags aspectMask) 
{
    VkCommandBuffer commandBuffer = AllocateCommandBuffer();
    BeginCommandBuffer(commandBuffer, true);

    VkBufferImageCopy region = 
    {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = 
        {
            .aspectMask = aspectMask,
            .mipLevel = 0,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
        .imageOffset = {0,0,0},
        .imageExtent = {dstImage.extent.width, dstImage.extent.height, 1},
    };

    vkCmdCopyBufferToImage(commandBuffer, srcBuffer.handle, dstImage.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

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
VkDescriptorSetLayout CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings) 
{
    VkDescriptorSetLayoutCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .bindingCount = (uint32_t)bindings.size(),
        .pBindings = bindings.begin(),
    };    

    VkDescriptorSetLayout setLayout;
    vkCreateDescriptorSetLayout(getDevice(), &createInfo, nullptr, &setLayout);
    return setLayout;
}

VkDescriptorPool CreateDescriptorPool(std::initializer_list<VkDescriptorPoolSize> poolSizes, uint32_t maxSets) 
{
    VkDescriptorPoolCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .maxSets = maxSets,
        .poolSizeCount = (uint32_t)poolSizes.size(),
        .pPoolSizes = poolSizes.begin(),
    };

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(getDevice(), &createInfo, nullptr, &descriptorPool);
    return descriptorPool;
}

VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool)
{
    VkDescriptorSetAllocateInfo allocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &setLayout,
    };

    VkDescriptorSet set;
    vkAllocateDescriptorSets(getDevice(), &allocateInfo, &set);
    return set;
}

VkPipelineLayout CreatePipelineLayout(std::initializer_list<VkDescriptorSetLayout> setLayouts, std::initializer_list<VkPushConstantRange> pushConstant)
{
    VkPipelineLayoutCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (uint32_t)setLayouts.size(),
        .pSetLayouts = setLayouts.begin(),
        .pushConstantRangeCount = (uint32_t)pushConstant.size(),
        .pPushConstantRanges = pushConstant.begin(),
    };

    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(getDevice(), &createInfo, nullptr, &pipelineLayout);
    return pipelineLayout;
}

Image CreateImage(const glm::uvec2& size, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkMemoryPropertyFlags memoryProperty)
{
    Image image;

    VkImageCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = VK_IMAGE_TYPE_2D,
        .format = format,
        .extent = 
        {
            .width = size.x,
            .height = size.y,
            .depth = 1,
        },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
    
    vkCreateImage(getDevice(), &createInfo, nullptr, &image.handle);

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(getDevice(), image.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .allocationSize = requirements.size,
        .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, memoryProperty),
    };

    vkAllocateMemory(getDevice(), &allocateInfo, nullptr, &image.memory);
    vkBindImageMemory(getDevice(), image.handle, image.memory, 0);

    image.memorySize = requirements.size;
    
    VkImageViewCreateInfo imageViewCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .image = image.handle,
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = format,
        .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
        .subresourceRange = 
        {
            .aspectMask = aspectMask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        },
    };

    image.extent = {size.x, size.y};

    vkCreateImageView(getDevice(), &imageViewCreateInfo, nullptr, &image.view);

    return image;
}