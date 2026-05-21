#pragma once
#include "Renderer/Descriptor.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/Types.hpp"
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>

class GraphicsPipeline
{
    public:
        void SetVertexShader(VkShaderModule shader);
        void SetFragmentShader(VkShaderModule shader);
        void SetGeometryShader(VkShaderModule shader);
        void SetTessellationShader(VkShaderModule shader);
        
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
        void SetFrontFace(FrontFace frontFace);
        void SetViewport(const VkViewport& viewport);
        
        void Create(const RenderPass& renderPass, uint32_t subpassIndex);
        void Destroy();  
        
        VkPipelineLayout GetPipelineLayout() const;
        VkPipeline GetHandle() const;

        template <typename... Descriptors>
        void AddDescriptors(const Descriptor &descriptor, const Descriptors &...descriptors)
        {
            static_assert((std::is_same_v<const Descriptors &, const Descriptor &> && ...), "Argument type must be descriptors");
            mSetLayouts.push_back(descriptor.GetDescriptorSetLayout());
            AddDescriptors(descriptors...);
        }

        void AddDescriptors(const Descriptor &descriptor)
        {
            mSetLayouts.push_back(descriptor.GetDescriptorSetLayout());
        }

        void SetPushConstant(ShaderStage stage, size_t size);

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
        VkViewport mViewport = {};

        VkPrimitiveTopology mPrimitive = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkSampleCountFlagBits mSampleCount = VK_SAMPLE_COUNT_1_BIT;
        VkFrontFace mFrontFace = VK_FRONT_FACE_CLOCKWISE;

        std::vector<VkDescriptorSetLayout> mSetLayouts;
        std::unordered_map<ShaderStage, VkPushConstantRange> mPushConstants;

        bool mDepthTestEnable = false;
        bool mDepthWriteEnable = false;
        bool mBlendEnable = false;
        bool mWireframeEnable = false;
};

