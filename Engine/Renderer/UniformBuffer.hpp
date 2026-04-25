#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
    public:
        void Create(size_t capacity);
        void SetData(size_t size, void* data);
        const Buffer& GetBuffer() const { return mBuffer; }
        size_t GetCapacity();
        
        UniformBuffer();
        ~UniformBuffer();
    private:
        Buffer mBuffer;
};