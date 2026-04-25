#pragma once
#include "Renderer/RenderPass.hpp"
#include "Renderer/Types.hpp"
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

        void EnableDepthTesting(bool enable);
        void EnableDepthWrite(bool enable);
        void EnableBlending(bool enable);
        void EnableWireframe(bool enable);

        void AddBinding(uint32_t binding, size_t stride, InputRate inputRate);
        void AddAttribute(uint32_t binding, uint32_t location, ImageFormat format, size_t offset);
        void AddColorBlendAttachment(bool enableBlending);

        void SetCullMode(CullMode cullMode);
        void SetPrimitive(PrimitiveType primitive);
        void SetMultisampleCount(SampleCount count);
        void SetViewport(const VkViewport& viewport);

        void SetPipelineLayout(VkPipelineLayout layout);

        void Create(const RenderPass& renderPass, uint32_t subpassIndex);

        VkPipelineLayout GetPipelineLayout() const;

        VkPipeline GetHandle() const { return mHandle; }

        void ClearAttributesAndBinding();
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

        bool mDepthTestEnable = false;
        bool mDepthWriteEnable = false;
        bool mBlendEnable = false;
        bool mWireframeEnable = false;
};