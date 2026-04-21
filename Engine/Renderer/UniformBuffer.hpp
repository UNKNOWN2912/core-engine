#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
    public:
        UniformBuffer();
        ~UniformBuffer();
        void Create(size_t capacity);
        void SetData(size_t size, void* data);
        void UpdateDescriptor(VkDescriptorSet descriptorSet, int binding);
        const Buffer& GetBuffer() const { return mBuffer; }
        
        size_t GetCapacity();


    private:
        Buffer mBuffer;
};