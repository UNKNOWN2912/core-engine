#include "UniformBuffer.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Utility.hpp"
#include <cstring>

UniformBuffer::UniformBuffer()
{
}

UniformBuffer::~UniformBuffer()
{
    sUniformBufferCount--;
}

void UniformBuffer::Create(VkDeviceSize capacity, VkShaderStageFlags shaderStage, uint32_t binding) 
{
    if(sDescriptorPool == VK_NULL_HANDLE)
    {
        VkDescriptorPoolSize poolSize = {};
        poolSize.descriptorCount = 50;
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        sDescriptorPool = CreateDescriptorPool({poolSize}, 50);
    }
    
    mBinding = binding;
    mBuffer = CreateBuffer(capacity, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    
    VkDescriptorSetLayoutBinding setLayoutBinding = {};
    setLayoutBinding.binding = mBinding;
    setLayoutBinding.descriptorCount = 1;
    setLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    setLayoutBinding.stageFlags = shaderStage;
    mSetLayout = CreateDescriptorSetLayout({setLayoutBinding});

    mSet = AllocateDescriptorSet(mSetLayout, sDescriptorPool);

    sUniformBufferCount++;
}

void UniformBuffer::SetData(VkDeviceSize size, void* data) 
{
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
    descriptorWrite.dstBinding = mBinding;
    descriptorWrite.dstSet = mSet;
    descriptorWrite.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(getDevice(), 1, &descriptorWrite, 0, nullptr);
}

VkDescriptorSet UniformBuffer::GetDescriptorSet() { return mSet; }
VkDescriptorSetLayout UniformBuffer::GetSetLayout() { return mSetLayout; }
uint32_t UniformBuffer::GetBinding() { return mBinding; }

VkDescriptorPool UniformBuffer::GetDescriptorPool() { return sDescriptorPool; };
uint32_t UniformBuffer::GetUniformBufferCount() { return sUniformBufferCount; };

VkDescriptorPool UniformBuffer::sDescriptorPool = VK_NULL_HANDLE;
uint32_t UniformBuffer::sUniformBufferCount = 0;
