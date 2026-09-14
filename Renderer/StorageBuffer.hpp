#pragma once
#include "Renderer/Utility.hpp"

class StorageBuffer
{
public:
    void SetData(void *data, size_t size);
    const Buffer &GetBuffer() const;

    StorageBuffer() = default;
    StorageBuffer(const StorageBuffer &) = delete;
    StorageBuffer &operator=(const StorageBuffer &) = delete;
    StorageBuffer(StorageBuffer &&buffer) noexcept;
    StorageBuffer &operator=(StorageBuffer &&buffer) noexcept;
    StorageBuffer(size_t size, void *data = nullptr);
    ~StorageBuffer();

private:
    void Create(void *data, size_t size);
    void Destroy();

    Buffer mBuffer;
    Buffer mStagingBuffer;
};