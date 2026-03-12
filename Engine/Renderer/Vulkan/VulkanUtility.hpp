#pragma once
#include <GLFW/glfw3.h>
#include <vector>
#include <vulkan/vulkan.h>
#include <cassert>


#define vkAssert(object) assert(object != VK_NULL_HANDLE)


VkAttachmentDescription populateAttachmentDescription(VkFormat format, VkImageLayout finalLayout, VkAttachmentLoadOp loadOp, VkAttachmentStoreOp storeOp, VkAttachmentLoadOp stencilLoadOp, VkAttachmentStoreOp stencilStoreOp, VkSampleCountFlagBits samples);
VkAttachmentReference populateAttachmentReference(uint32_t index, VkImageLayout imageLayout);
VkSubpassDescription populateSubpassDescription(const std::vector<VkAttachmentReference>& colorAttachments, const std::vector<VkAttachmentReference>& inputAttachments, const VkAttachmentReference& depthAttachment, VkPipelineBindPoint pipelineBindPoint);
VkSubpassDescription populateSubpassDescription(const std::vector<VkAttachmentReference>& colorAttachments, const std::vector<VkAttachmentReference>& inputAttachments, VkPipelineBindPoint pipelineBindPoint);
VkSubpassDependency populateSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask);
VkRenderPass createRenderPass(VkDevice device);
VkCommandPool createCommandPool(VkDevice device);
VkDescriptorPool createDescriptorPool(VkDevice device, const std::vector<VkDescriptorPoolSize>& poolSizes, uint32_t maxAllocation);
VkCommandBuffer allocateCommandBuffers(VkDevice device, VkCommandPool commandPool, VkBool32 secondary = VK_FALSE);
VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
VkSemaphore createSemaphore(VkDevice device);
VkFence createFence(VkDevice device, VkBool32 signaled = VK_FALSE);
VkFramebuffer createFramebuffer(VkDevice device, VkRenderPass renderPass, VkExtent2D extent, const std::vector<VkImageView>& imageViewAttachments);
VkShaderModule createShaderModuleFromMemory(VkDevice device, const std::vector<uint32_t>& code);
VkShaderModule createShaderFromFile(VkDevice device, const char* filename);
VkPipelineLayout createPipelineLayout(VkDevice device, const std::vector<VkDescriptorSetLayout>& setLayouts, const std::vector<VkPushConstantRange>& pushContantRanges);
uint32_t getMemoryTypeIndex(VkPhysicalDevice physicalDevice, uint32_t memoryTypeBits, VkMemoryPropertyFlags memoryProperty);
VkBuffer createBuffer(VkDevice device, VkBufferUsageFlags usage, VkDeviceSize size);
VkSampler createSampler(VkDevice device, VkFilter minFilter, VkFilter magFilter, VkSamplerAddressMode addressMode, uint32_t anisotropy);
VkDeviceMemory allocateMemoryForBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBuffer buffer, VkMemoryPropertyFlags memoryProperty);
VkDeviceMemory allocateMemoryForImage(VkPhysicalDevice physicalDevice, VkDevice device, VkImage image, VkMemoryPropertyFlags memoryProperty);
VkDescriptorSet allocateDescriptorSet(VkDevice device, VkDescriptorPool descriptorPool, VkDescriptorSetLayout setLayout);
void submitCommandBuffer(VkQueue queue, VkCommandBuffer commandBuffer, VkSemaphore waitSemaphore, VkSemaphore signalSemaphore, VkFence fence, VkPipelineStageFlags waitFlag);
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
VkImage createImage2D(VkDevice device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

void copyBufferToImage(VkDevice device, VkCommandPool commandPool, VkQueue transferQueue, uint32_t width, uint32_t height, VkBuffer buffer, VkImage image, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);
void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue transferQueue, VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, VkImageAspectFlags aspectMask = VK_IMAGE_ASPECT_COLOR_BIT);

void freeCommandBuffers(VkDevice device, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
void destroyInstance(VkInstance instance);
void destroyLogicalDevice(VkDevice device);
void destroyRenderPass(VkDevice device, VkRenderPass renderPass);
void destroyCommandPool(VkDevice device, VkCommandPool commandpool);
void destroyDescriptorPool(VkDevice device, VkDescriptorPool descriptorpool);
void destroyGraphicPipeline(VkDevice device, VkPipeline pipeline);
void destroyWindowSurface(VkInstance instance, VkSurfaceKHR surface);