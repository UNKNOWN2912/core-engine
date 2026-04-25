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
    mBuffer = CreateBuffer(capacity, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void UniformBuffer::SetData(size_t size, void* data)
{
    CHROME_TRACE_FUNCTION();
    vkDeviceWaitIdle(getDevice());
    
    memcpy(mBuffer.map, data, size);
}
