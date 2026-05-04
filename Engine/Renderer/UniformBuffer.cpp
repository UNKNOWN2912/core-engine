#include "UniformBuffer.hpp"
#include "Core/Macro.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Utility.hpp"
#include <cstring>

UniformBuffer::UniformBuffer()
{
    CHROME_TRACE_FUNCTION();
}

UniformBuffer::~UniformBuffer()
{
    CHROME_TRACE_FUNCTION();
}

void UniformBuffer::Create(size_t capacity) 
{
    CHROME_TRACE_FUNCTION();
    mBuffer = CreateBuffer(capacity, BufferUsage::UniformBuffer, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
}

void UniformBuffer::SetData(size_t size, void* data)
{
    CHROME_TRACE_FUNCTION();
    vkDeviceWaitIdle(getDevice());
    
    memcpy(mBuffer.map, data, size);
}
