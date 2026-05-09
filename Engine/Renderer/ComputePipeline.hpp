#pragma once
#include "Renderer/Descriptor.hpp"
#include <string_view>
#include <vulkan/vulkan.h>

class ComputePipeline
{
    public:
        void Create(const std::vector<Descriptor*>& descriptors);
        void Destroy();

        void LoadShader(std::string_view filename);
        
        VkPipelineLayout GetPipelineLayout() const;
        VkPipeline GetHandle() const;

        ~ComputePipeline();
    private:
        VkPipeline mHandle = VK_NULL_HANDLE;
        VkShaderModule mShader = VK_NULL_HANDLE;
        VkPipelineLayout mLayout = VK_NULL_HANDLE;

};