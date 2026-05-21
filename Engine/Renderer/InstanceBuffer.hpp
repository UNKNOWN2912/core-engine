#pragma once

#include "Renderer/Utility.hpp"
class InstanceBuffer
{
    public:
        void SetData(void* data, size_t size);
        void Destroy();

        const Buffer& GetBuffer() const { return mBuffer; }
    private:
        friend class Renderer;
        Buffer mStagingBuffer;
        Buffer mBuffer;

        size_t mSize = 0;
};