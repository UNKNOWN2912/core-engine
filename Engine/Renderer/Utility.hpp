#pragma once
#include "Renderer/Types.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct Buffer
{
    VkBuffer handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    size_t size = 0;
    void* map = nullptr;
};

struct Image
{
    VkImage handle = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkDeviceSize memorySize = 0;
    glm::uvec2 size = {0, 0};
};

Buffer CreateBuffer(size_t size, BufferUsage usage, MemoryProperty memoryProperties);
void DestroyBuffer(Buffer& buffer);
void TransferBufferData(const Buffer& srcBuffer, Buffer& dstBuffer); 

Image CreateImage(const glm::uvec2& size, ImageFormat format, ImageUsage usage, ImageAspect aspect, MemoryProperty memoryProperty);
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

void TransitionImageLayout(ImageLayout oldLayout, ImageLayout newLayout, ImageAspect aspectMask, const Image& image);
void TransferImageData(const Buffer& srcBuffer, Image& dstImage, ImageAspect aspectMask);
