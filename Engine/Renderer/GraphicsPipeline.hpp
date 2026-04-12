#pragma once
#include <string_view>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

class GraphicsPipeline
{
    public:
        void LoadVertexShader(std::string_view filename);
        void LoadFragmentShader(std::string_view filename);
        void LoadGeometryShader(std::string_view filename);
        void LoadTessellationShader(std::string_view filename);

        void EnableDepth(bool enable);
        void EnableWireframe(bool enable);

        void AddBinding(uint32_t binding, size_t stride, VkVertexInputRate inputRate);
        void AddAttribute(uint32_t binding, uint32_t location, VkFormat format, size_t offset);
        void AddColorBlendAttachment();

        void SetCullMode(VkCullModeFlags cullMode);
        void SetPrimitive(VkPrimitiveTopology primitive);
        void SetMultisampleCount(uint32_t count);
        void SetViewport(const VkViewport& viewport);

        void Create(VkRenderPass renderPass, uint32_t subpassIndex);

        VkPipeline GetHandle() const { return mHandle; }
    private:
        VkShaderModule mVertexShader = VK_NULL_HANDLE;
        VkShaderModule mFragmentShader = VK_NULL_HANDLE;
        VkShaderModule mGeometryShader = VK_NULL_HANDLE;
        VkShaderModule mTessellationShader = VK_NULL_HANDLE;

        VkPipeline mHandle = VK_NULL_HANDLE;
        VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;

        std::vector<VkVertexInputBindingDescription> mBindingDescription;
        std::vector<VkVertexInputAttributeDescription> mAttributeDescription;
        std::vector<VkPipelineColorBlendAttachmentState> mColorBlendStates;

        VkCullModeFlags mCullMode = VK_CULL_MODE_BACK_BIT;
        VkPrimitiveTopology mPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkSampleCountFlagBits mSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkViewport mViewport = {};

        bool mDepthEnable = false;
        bool mWireframeEnable = false;

};