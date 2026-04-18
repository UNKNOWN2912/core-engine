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

void UniformBuffer::Create(VkDeviceSize capacity, VkShaderStageFlags shaderStage, uint32_t binding) 
{
    CHROME_TRACE_FUNCTION();
    mBuffer = CreateBuffer(capacity, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void UniformBuffer::SetDataToDescriptor(VkDeviceSize size, void* data, VkDescriptorSet descriptorSet, int binding)
{
    CHROME_TRACE_FUNCTION();
    vkDeviceWaitIdle(getDevice());
    
    memcpy(mBuffer.map, data, size);

    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = mBuffer.handle;
    bufferInfo.offset = 0;
    bufferInfo.range = mBuffer.size;

    VkWriteDescriptorSet descriptorWrite = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstSet = descriptorSet;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(getDevice(), 1, &descriptorWrite, 0, nullptr);
}
