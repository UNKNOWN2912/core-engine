#include "StorageBuffer.hpp"
#include <cstring>

StorageBuffer &StorageBuffer::operator=(StorageBuffer &&buffer) noexcept
{
    Destroy();

    mBuffer = buffer.mBuffer;
    mStagingBuffer = buffer.mStagingBuffer;

    buffer.mBuffer = {};
    buffer.mStagingBuffer = {};

    return *this;
}

StorageBuffer::StorageBuffer(StorageBuffer &&buffer) noexcept
{
    mBuffer = buffer.mBuffer;
    mStagingBuffer = buffer.mStagingBuffer;

    buffer.mBuffer = {};
    buffer.mStagingBuffer = {};
}

StorageBuffer::StorageBuffer(size_t size, void *data)
{
    Create(data, size);
}

StorageBuffer::~StorageBuffer()
{
    Destroy();
}

void StorageBuffer::Create(void *data, size_t size)
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

void StorageBuffer::Destroy()
{
    DestroyBuffer(mStagingBuffer);
    DestroyBuffer(mBuffer);
}

const Buffer &StorageBuffer::GetBuffer() const
{
    return mBuffer;
}
