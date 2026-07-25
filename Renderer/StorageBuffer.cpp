#include "StorageBuffer.hpp"
#include <cstring>

void StorageBuffer::CreateStorageBuffer(void *data, size_t size)
{
    mStagingBuffer = CreateBuffer(size, BufferUsage::TransferSource, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
    mBuffer = CreateBuffer(size, BufferUsage::Storage | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);

    if (data != nullptr)
    {
        SetData(data, size);
    }
}

void StorageBuffer::SetData(void *data, size_t size)
{
    memcpy(mStagingBuffer.map, data, size);
    TransferBufferData(mStagingBuffer, mBuffer);
}

void StorageBuffer::DestroyStorageBuffer()
{
    DestroyBuffer(mStagingBuffer);
    DestroyBuffer(mBuffer);
}

void StorageBuffer::Resize(void *data, size_t size)
{
    DestroyStorageBuffer();
    CreateStorageBuffer(data, size);
}

const Buffer &StorageBuffer::GetBuffer() const
{
    return mBuffer;
}