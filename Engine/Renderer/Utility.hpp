#pragma once
#include "Renderer/Types.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkDeviceSize size = 0;
    void* map = nullptr;
};

struct Image
{
    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceSize memorySize = 0;
    VkExtent2D extent = {0, 0};
};

Buffer CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
void DestroyBuffer(Buffer& buffer);
void TransferBufferData(const Buffer& srcBuffer, Buffer& dstBuffer); 

Image CreateImage(const glm::uvec2& size, VkFormat format, VkImageUsageFlags usage, VkImageAspectFlags aspectMask, VkMemoryPropertyFlags memoryProperty);
void DestroyImage(Image& image);

VkImageView CreateImageView(VkImage image, ImageFormat format, ImageAspect aspect);


VkCommandBuffer AllocateCommandBuffer(VkCommandPool commandPool);
void BeginCommandBuffer(VkCommandBuffer commandBuffer, bool singleUse);
void EndCommandBuffer(VkCommandBuffer commandBuffer);
void ExecuteCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);
VkShaderModule CreateShaderModuleFromMemory(VkDevice device, const std::vector<uint32_t> &code);
VkShaderModule CreateShaderFromFile(VkDevice device, const char *filename);

VkDescriptorSetLayout CreateDescriptorSetLayout(std::initializer_list<VkDescriptorSetLayoutBinding> bindings);
VkDescriptorPool CreateDescriptorPool(std::initializer_list<VkDescriptorPoolSize> sizes, uint32_t maxSets);
VkDescriptorSet AllocateDescriptorSet(VkDescriptorSetLayout setLayout, VkDescriptorPool descriptorPool);
VkPipelineLayout CreatePipelineLayout(std::initializer_list<VkDescriptorSetLayout> setLayouts, std::initializer_list<VkPushConstantRange> pushConstant);

void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask, Image& image);
void TransferImageData(const Buffer& srcBuffer, Image& dstImage, VkImageAspectFlags aspectMask);
