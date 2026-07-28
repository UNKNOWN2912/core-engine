#include "Utility.hpp"
#include "Core/Macro.hpp"
#include "GraphicsContext.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/Types.hpp"

uint32_t FindMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags memoryProperties)
{
    CHROME_TRACE_FUNCTION();

    VkPhysicalDeviceMemoryProperties properties;
    vkGetPhysicalDeviceMemoryProperties(GraphicsContext::GetPhysicalDevice(), &properties);

    for (uint32_t i = 0; i < properties.memoryTypeCount; i++)
    {
        bool isTypeBit = (typeBits & (1 << i)) != 0;
        bool isMemoryProperty = (memoryProperties & properties.memoryTypes[i].propertyFlags) == memoryProperties;

        if (isTypeBit && isMemoryProperty)
        {
            return i;
        }
    }

    return UINT32_MAX;
}

Buffer CreateBuffer(size_t size, BufferUsage usage, MemoryProperty memoryProperties)
{
    CHROME_TRACE_FUNCTION();
    Buffer buffer;

    VkBufferCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .size = size,
            .usage = GetVulkanBufferUsage(usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        };

    vkCreateBuffer(GraphicsContext::GetDevice(), &createInfo, nullptr, &buffer.handle);

    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(GraphicsContext::GetDevice(), buffer.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = requirements.size,
            .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, GetVulkanMemoryProperty(memoryProperties)),
        };

    vkAllocateMemory(GraphicsContext::GetDevice(), &allocateInfo, nullptr, &buffer.memory);

    buffer.capacity = size;

    vkBindBufferMemory(GraphicsContext::GetDevice(), buffer.handle, buffer.memory, 0);

    if ((GetVulkanMemoryProperty(memoryProperties) & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        vkMapMemory(GraphicsContext::GetDevice(), buffer.memory, 0, requirements.size, 0, &buffer.map);
    }

    return buffer;
}

void DestroyBuffer(Buffer &buffer)
{
    CHROME_TRACE_FUNCTION();
    if (buffer.handle == VK_NULL_HANDLE)
        return;
    vkDestroyBuffer(GraphicsContext::GetDevice(), buffer.handle, nullptr);
    vkFreeMemory(GraphicsContext::GetDevice(), buffer.memory, nullptr);
    buffer = {};
}

VkCommandPool CreateCommandPool()
{
    CHROME_TRACE_FUNCTION();

    VkCommandPoolCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        };

    VkCommandPool commandPool;
    vkCreateCommandPool(GraphicsContext::GetDevice(), &createInfo, nullptr, &commandPool);
    return commandPool;
}

void TransferBufferData(const Buffer &srcBuffer, Buffer &dstBuffer)
{
    CHROME_TRACE_FUNCTION();
    VkCommandPool commandPool = CreateCommandPool();
    VkCommandBuffer commandBuffer = AllocateCommandBuffer(commandPool);
    BeginCommandBuffer(commandBuffer, true);

    VkBufferCopy region =
        {
            .srcOffset = 0,
            .dstOffset = 0,
            .size = srcBuffer.capacity,
        };

    vkCmdCopyBuffer(commandBuffer, srcBuffer.handle, dstBuffer.handle, 1, &region);

    EndCommandBuffer(commandBuffer);
    ExecuteCommandBuffer(commandBuffer, GraphicsContext::GetQueues().transfer);

    vkQueueWaitIdle(GraphicsContext::GetQueues().transfer);

    vkFreeCommandBuffers(GraphicsContext::GetDevice(), commandPool, 1, &commandBuffer);

    vkDestroyCommandPool(GraphicsContext::GetDevice(), commandPool, nullptr);
}

void TransitionImageLayout(ImageLayout oldLayout, ImageLayout newLayout, ImageAspect aspectMask, const ImageDeprecated &image)
{
    CHROME_TRACE_FUNCTION();

    VkCommandBuffer commandBuffer = AllocateCommandBuffer(GraphicsContext::GetCommandPool());
    BeginCommandBuffer(commandBuffer, true);

    VkImageMemoryBarrier barrier =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
            .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
            .dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
            .oldLayout = GetVulkanImageLayout(oldLayout),
            .newLayout = GetVulkanImageLayout(newLayout),
            .image = image.handle,
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspectMask),
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

    EndCommandBuffer(commandBuffer);
    ExecuteCommandBuffer(commandBuffer, GraphicsContext::GetQueues().transfer);
}

void TransferImageData(const Buffer &srcBuffer, ImageDeprecated &dstImage, ImageAspect aspectMask)
{
    CHROME_TRACE_FUNCTION();
    VkCommandBuffer commandBuffer = AllocateCommandBuffer(GraphicsContext::GetCommandPool());
    BeginCommandBuffer(commandBuffer, true);

    VkBufferImageCopy region =
        {
            .bufferOffset = 0,
            .bufferRowLength = 0,
            .bufferImageHeight = 0,
            .imageSubresource =
                {
                    .aspectMask = GetVulkanImageAspect(aspectMask),
                    .mipLevel = 0,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
            .imageOffset = {0, 0, 0},
            .imageExtent = {dstImage.size.x, dstImage.size.y, 1},
        };

    vkCmdCopyBufferToImage(commandBuffer, srcBuffer.handle, dstImage.handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    EndCommandBuffer(commandBuffer);
    ExecuteCommandBuffer(commandBuffer, GraphicsContext::GetQueues().transfer);

    vkQueueWaitIdle(GraphicsContext::GetQueues().transfer);

    vkFreeCommandBuffers(GraphicsContext::GetDevice(), GraphicsContext::GetCommandPool(), 1, &commandBuffer);
}

VkCommandBuffer AllocateCommandBuffer(VkCommandPool commandPool)
{
    CHROME_TRACE_FUNCTION();
    VkCommandBufferAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(GraphicsContext::GetDevice(), &allocateInfo, &commandBuffer);
    return commandBuffer;
}

void BeginCommandBuffer(VkCommandBuffer commandBuffer, bool singleUse)
{
    CHROME_TRACE_FUNCTION();
    VkCommandBufferBeginInfo beginInfo =
        {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = (singleUse) ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : (VkCommandBufferUsageFlags)0,
        };

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void EndCommandBuffer(VkCommandBuffer commandBuffer)
{
    CHROME_TRACE_FUNCTION();
    vkEndCommandBuffer(commandBuffer);
}

void ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue)
{
    CHROME_TRACE_FUNCTION();

    VkSubmitInfo submitInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
        };

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
}

VkShaderModule CreateShaderModuleFromMemory(VkDevice device, const std::vector<uint32_t> &code)
{
    CHROME_TRACE_FUNCTION();

    VkShaderModuleCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
            .codeSize = code.size(),
            .pCode = code.data(),
        };

    VkShaderModule shaderModule;
    vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    return shaderModule;
}

VkShaderModule CreateShaderFromFile(VkDevice device, const char *filename)
{
    CHROME_TRACE_FUNCTION();

    std::vector<uint32_t> code;
    FILE *fp = fopen(filename, "rb");
    if (fp == nullptr)
    {
        ERROR("Failed to find file: {}", filename);
    }

    fseek(fp, 0L, SEEK_END);
    int64_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    code.resize(size);
    fread(code.data(), sizeof(uint32_t), size / sizeof(uint32_t), fp);
    fclose(fp);

    return CreateShaderModuleFromMemory(device, code);
}
VkDescriptorSetLayout CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings)
{
    CHROME_TRACE_FUNCTION();
    VkDescriptorSetLayoutCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .bindingCount = (uint32_t)bindings.size(),
            .pBindings = bindings.begin(),
        };

    VkDescriptorSetLayout setLayout;
    vkCreateDescriptorSetLayout(GraphicsContext::GetDevice(), &createInfo, nullptr, &setLayout);
    return setLayout;
}

VkDescriptorPool CreateDescriptorPool(std::initializer_list<VkDescriptorPoolSize> poolSizes, uint32_t maxSets)
{
    CHROME_TRACE_FUNCTION();
    VkDescriptorPoolCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .maxSets = maxSets,
            .poolSizeCount = (uint32_t)poolSizes.size(),
            .pPoolSizes = poolSizes.begin(),
        };

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(GraphicsContext::GetDevice(), &createInfo, nullptr, &descriptorPool);
    return descriptorPool;
}

VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool)
{
    CHROME_TRACE_FUNCTION();

    VkDescriptorSetAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &setLayout,
        };

    VkDescriptorSet set;
    vkAllocateDescriptorSets(GraphicsContext::GetDevice(), &allocateInfo, &set);
    return set;
}

VkPipelineLayout CreatePipelineLayout(std::initializer_list<VkDescriptorSetLayout> setLayouts, std::initializer_list<VkPushConstantRange> pushConstant)
{
    CHROME_TRACE_FUNCTION();

    VkPipelineLayoutCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = (uint32_t)setLayouts.size(),
            .pSetLayouts = setLayouts.begin(),
            .pushConstantRangeCount = (uint32_t)pushConstant.size(),
            .pPushConstantRanges = pushConstant.begin(),
        };

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    vkCreatePipelineLayout(GraphicsContext::GetDevice(), &createInfo, nullptr, &pipelineLayout);
    return pipelineLayout;
}

ImageDeprecated CreateImage(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount, uint32_t layerCount, uint32_t mipmapCount)
{
    CHROME_TRACE_FUNCTION();

    ImageDeprecated image;

    VkImageCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = GetVulkanImageFormat(format),
            .extent =
                {
                    .width = size.x,
                    .height = size.y,
                    .depth = 1,
                },
            .mipLevels = mipmapCount,
            .arrayLayers = layerCount,
            .samples = GetVulkanSampleCount(sampleCount),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = GetVulkanImageUsage(usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

    vkCreateImage(GraphicsContext::GetDevice(), &createInfo, nullptr, &image.handle);

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(GraphicsContext::GetDevice(), image.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = requirements.size,
            .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, GetVulkanMemoryProperty(memoryProperty)),
        };

    VK_CHECK(vkAllocateMemory(GraphicsContext::GetDevice(), &allocateInfo, nullptr, &image.memory));
    vkBindImageMemory(GraphicsContext::GetDevice(), image.handle, image.memory, 0);

    image.memorySize = requirements.size;

    VkImageViewType type = (layerCount == 1) ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_2D_ARRAY;

    VkComponentMapping mapping = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};

    if (format == ImageFormat::R8 || format == ImageFormat::R8UNORM)
    {
        mapping = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_R};
    }

    VkImageViewCreateInfo imageViewCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image.handle,
            .viewType = type,
            .format = GetVulkanImageFormat(format),
            .components = mapping,
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspect),
                    .baseMipLevel = 0,
                    .levelCount = mipmapCount,
                    .baseArrayLayer = 0,
                    .layerCount = layerCount,
                },
        };

    image.size = {size.x, size.y};

    vkCreateImageView(GraphicsContext::GetDevice(), &imageViewCreateInfo, nullptr, &image.view);
    image.format = format;
    return image;
}

ImageDeprecated CreateCubeMapImage(const glm::uvec2 &size, ImageFormat format, ImageUsage usage, ImageAspect aspect, MemoryProperty memoryProperty, SampleCount sampleCount)
{
    CHROME_TRACE_FUNCTION();

    ImageDeprecated image;

    VkImageCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
            .imageType = VK_IMAGE_TYPE_2D,
            .format = GetVulkanImageFormat(format),
            .extent =
                {
                    .width = size.x,
                    .height = size.y,
                    .depth = 1,
                },
            .mipLevels = 1,
            .arrayLayers = 6,
            .samples = GetVulkanSampleCount(sampleCount),
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = GetVulkanImageUsage(usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        };

    vkCreateImage(GraphicsContext::GetDevice(), &createInfo, nullptr, &image.handle);

    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(GraphicsContext::GetDevice(), image.handle, &requirements);

    VkMemoryAllocateInfo allocateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            .allocationSize = requirements.size,
            .memoryTypeIndex = FindMemoryTypeIndex(requirements.memoryTypeBits, GetVulkanMemoryProperty(memoryProperty)),
        };

    VK_CHECK(vkAllocateMemory(GraphicsContext::GetDevice(), &allocateInfo, nullptr, &image.memory));
    vkBindImageMemory(GraphicsContext::GetDevice(), image.handle, image.memory, 0);

    image.memorySize = requirements.size;

    VkImageViewCreateInfo imageViewCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image.handle,
            .viewType = VK_IMAGE_VIEW_TYPE_CUBE,
            .format = GetVulkanImageFormat(format),
            .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspect),
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 6,
                },
        };

    image.size = {size.x, size.y};
    image.format = format;

    vkCreateImageView(GraphicsContext::GetDevice(), &imageViewCreateInfo, nullptr, &image.view);

    return image;
}
void DestroyImage(ImageDeprecated &image)
{
    vkDestroyImageView(GraphicsContext::GetDevice(), image.view, nullptr);
    vkDestroyImage(GraphicsContext::GetDevice(), image.handle, nullptr);
    vkFreeMemory(GraphicsContext::GetDevice(), image.memory, nullptr);

    image = {};
}

VkImageView CreateImageView(VkImage image, ImageFormat format, ImageAspect aspect)
{
    VkImageViewCreateInfo imageViewCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = GetVulkanImageFormat(format),
            .components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY},
            .subresourceRange =
                {
                    .aspectMask = GetVulkanImageAspect(aspect),
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                },
        };

    VkImageView view;
    vkCreateImageView(GraphicsContext::GetDevice(), &imageViewCreateInfo, nullptr, &view);
    return view;
}
