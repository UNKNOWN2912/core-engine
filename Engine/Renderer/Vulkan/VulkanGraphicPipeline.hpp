#pragma once
#include <vector>
#include <vulkan/vulkan.h>

class VulkanGraphicPipeline
{
  public:
    VulkanGraphicPipeline();
    void SetVertexShader(VkShaderModule shaderModule);
    void SetFragmentShader(VkShaderModule shaderModule);

    VkPipeline GetHandle();

    void EnableWireFrame(VkBool32 enable);
    void EnableDepthTesting(VkBool32 enable);

    void Create(VkDevice device, VkRenderPass renderPass, uint32_t subpass, VkPipelineLayout pipelineLayout);
    void AddVertexAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format);
    void AddVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

    void AddColorBlendAttachment();

  private:
    std::vector<VkPipelineColorBlendAttachmentState> mColorBlendAttachments;
    std::vector<VkVertexInputAttributeDescription> mAttributes;
    std::vector<VkVertexInputBindingDescription> mBindings;
    VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule mFragmentShaderModule = VK_NULL_HANDLE;
    VkPipeline mHandle = VK_NULL_HANDLE;
    VkBool32 mDepthTestingEnabled;
    VkBool32 mWireFrameEnabled;
    VkDevice mDevice = VK_NULL_HANDLE;
};