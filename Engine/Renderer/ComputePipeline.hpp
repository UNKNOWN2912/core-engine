#pragma once
#include "Renderer/Descriptor.hpp"
#include <string_view>
#include <vulkan/vulkan.h>

class ComputePipeline
{
    public:
        void Create(std::initializer_list<Descriptor> descriptors);
        void LoadShader(std::string_view filename);
        
        VkPipelineLayout GetPipelineLayout() const;
        VkPipeline GetHandle() const;
    private:
        VkPipeline mHandle = VK_NULL_HANDLE;
        VkShaderModule mShader = VK_NULL_HANDLE;
        VkPipelineLayout mLayout = VK_NULL_HANDLE;

};