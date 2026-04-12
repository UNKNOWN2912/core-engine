#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
    void* map = nullptr;
};

Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
void DestroyBuffer(Buffer& buffer);
void TransferBufferData(const Buffer& srcBuffer, Buffer& dstBuffer); 

VkCommandBuffer AllocateCommandBuffer();
void BeginCommandBuffer(VkCommandBuffer commandBuffer, bool singleUse);
void EndCommandBuffer(VkCommandBuffer commandBuffer);
void ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);
VkShaderModule CreateShaderModuleFromMemory(VkDevice device, const std::vector<uint32_t> &code);
VkShaderModule CreateShaderFromFile(VkDevice device, const char *filename);
