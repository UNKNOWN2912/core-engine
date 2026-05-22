#include "UniformBuffer.hpp"
#include "Core/Macro.hpp"
#include "Utility.hpp"
#include <cstring>

UniformBuffer::~UniformBuffer()
{
    CHROME_TRACE_FUNCTION();
    Destroy();
}

UniformBuffer::UniformBuffer(size_t size)
{
    CHROME_TRACE_FUNCTION();
    mBuffer = CreateBuffer(size, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
}

UniformBuffer::UniformBuffer(size_t size, void *data)
{
    CHROME_TRACE_FUNCTION();
    mBuffer = CreateBuffer(size, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
    SetData(data);
}

UniformBuffer::UniformBuffer(UniformBuffer &&uniformBuffer) noexcept : mBuffer(uniformBuffer.mBuffer)
{
    uniformBuffer.mBuffer = {};
}

UniformBuffer &UniformBuffer::operator=(UniformBuffer &&uniformBuffer) noexcept
{
    mBuffer = uniformBuffer.mBuffer;
    uniformBuffer.mBuffer = {};

    return *this;
}

void UniformBuffer::Destroy()
{
    DestroyBuffer(mBuffer);
}

void UniformBuffer::SetData(void *data)
{
    CHROME_TRACE_FUNCTION();
    memcpy(mBuffer.map, data, mBuffer.size);
}

const Buffer &UniformBuffer::GetBuffer() const
{
    return mBuffer;
}
