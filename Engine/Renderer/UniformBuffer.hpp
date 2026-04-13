#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
    public:
        UniformBuffer();
        ~UniformBuffer();
        void Create(VkDeviceSize capacity, VkShaderStageFlags shaderStage, uint32_t binding);
        void SetData(VkDeviceSize size, void* data);

        const Buffer& GetBuffer() const { return mBuffer; }
        
        uint32_t GetBinding();
        VkDescriptorSet GetDescriptorSet();
        VkDeviceSize GetCapacity();
        VkDescriptorSetLayout GetSetLayout();
        static uint32_t GetUniformBufferCount();
        static VkDescriptorPool GetDescriptorPool();


    private:

        Buffer mBuffer;

        VkDescriptorSet mSet = VK_NULL_HANDLE;
        VkDescriptorSetLayout mSetLayout = VK_NULL_HANDLE;
        uint32_t mBinding = 0;
        static VkDescriptorPool sDescriptorPool;
        static uint32_t sUniformBufferCount;
};