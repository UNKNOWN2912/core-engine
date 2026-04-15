#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
    public:
        UniformBuffer();
        ~UniformBuffer();
        void Create(VkDeviceSize capacity, VkShaderStageFlags shaderStage, uint32_t binding);
        void SetDataToDescriptor(VkDeviceSize size, void* data, VkDescriptorSet descriptorSet, int binding);
        const Buffer& GetBuffer() const { return mBuffer; }
        
        VkDeviceSize GetCapacity();


    private:
        Buffer mBuffer;
};