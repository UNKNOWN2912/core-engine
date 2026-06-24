#pragma once
#include "Renderer/Utility.hpp"

class StorageBuffer
{
public:
    void CreateStorageBuffer(void *data, size_t size);
    void SetData(void *data, size_t size);
    void DestroyStorageBuffer();
    void Resize(void *data, size_t size);
    const Buffer &GetBuffer() const;

private:
    Buffer mBuffer;
    Buffer mStagingBuffer;
};