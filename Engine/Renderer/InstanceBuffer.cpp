#include "InstanceBuffer.hpp"
#include "Core/Macro.hpp"
#include <memory.h>

void InstanceBuffer::SetData(void* data, size_t size)
{
    CHROME_TRACE_FUNCTION();
    if(mStagingBuffer.size < size || mStagingBuffer.handle == VK_NULL_HANDLE)
    {
        mStagingBuffer = CreateBuffer(size, BufferUsage::TransferSource, MemoryProperty::HostVisible | MemoryProperty::HostCoherent);
    }

    if(mBuffer.size < size ||  mBuffer.handle == VK_NULL_HANDLE)
    {
        mBuffer = CreateBuffer(size, BufferUsage::VertexBuffer | BufferUsage::Storage | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);
    }

    if(data != nullptr)
        memcpy(mStagingBuffer.map, data, size);
    else
        memset(mStagingBuffer.map, 0, size);

    TransferBufferData(mStagingBuffer, mBuffer);
}

void InstanceBuffer::Destroy() 
{
    DestroyBuffer(mBuffer);
    DestroyBuffer(mStagingBuffer);

    mBuffer = {};
    mStagingBuffer = {};
    mSize = 0;
}
