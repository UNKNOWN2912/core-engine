#include "ComputePipeline.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Utility.hpp"

void ComputePipeline::Create(std::initializer_list<Descriptor> descriptors) 
{

    std::vector<VkDescriptorSetLayout> setLayouts;

    for (const Descriptor& des : descriptors) 
    {
        setLayouts.push_back(des.GetDescriptorSetLayout());
    }

    VkPipelineLayoutCreateInfo pipelineCreateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .setLayoutCount = (uint32_t)setLayouts.size(),
        .pSetLayouts = setLayouts.data(),
    };

    vkCreatePipelineLayout(getDevice(), &pipelineCreateInfo, nullptr, &mLayout);

    VkPipelineShaderStageCreateInfo stage = 
    {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = mShader,
        .pName = "main",
    };

    VkComputePipelineCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .stage = stage,
        .layout = mLayout,
    };

    vkCreateComputePipelines(getDevice(), VK_NULL_HANDLE, 1, &createInfo, nullptr, &mHandle);
}

VkPipelineLayout ComputePipeline::GetPipelineLayout() const 
{
    return mLayout;    
}

VkPipeline ComputePipeline::GetHandle() const 
{
    return mHandle; 
}

void ComputePipeline::LoadShader(std::string_view filename) 
{
    mShader = CreateShaderFromFile(getDevice(), filename.data());
}

