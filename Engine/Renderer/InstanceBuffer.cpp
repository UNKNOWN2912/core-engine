#include "InstanceBuffer.hpp"
#include "Core/Macro.hpp"
#include <memory.h>

void InstanceBuffer::SetData(void* data, size_t size)
{
    CHROME_TRACE_FUNCTION();
    if(mStagingBuffer.size < size || mStagingBuffer.handle == VK_NULL_HANDLE)
    {
        mStagingBuffer = CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }

    if(mBuffer.size < size ||  mBuffer.handle == VK_NULL_HANDLE)
    {
        mBuffer = CreateBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    }

    memcpy(mStagingBuffer.map, data, size);

    TransferBufferData(mStagingBuffer, mBuffer);
}
