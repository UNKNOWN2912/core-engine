#include "VertexBuffer.hpp"

void VertexBuffer::Create(VkDeviceSize capacity)
{
    mCapacity = capacity;
    mStagingBuffer = createBuffer(getDevice(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, mCapacity);
    mStagingMemory = allocateMemoryForBuffer(getPhysicalDevice(), getDevice(), mStagingBuffer, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    mHandle = createBuffer(getDevice(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mCapacity);
    mMemory = allocateMemoryForBuffer(getPhysicalDevice(), getDevice(), mHandle, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    vkMapMemory(getDevice(), mStagingMemory, 0, mCapacity, 0, &mStageMemoryMap);
}

void VertexBuffer::StageData(size_t size, void *data)
{
    memcpy(mStageMemoryMap, data, size);
}

void VertexBuffer::PushData(VkDeviceSize size)
{
    VkCommandBuffer transferCommandBuffer = allocateCommandBuffers(getDevice(), getCommandPool());

    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(transferCommandBuffer, &beginInfo);

    VkBufferCopy region = {};
    region.size = size;

    vkCmdCopyBuffer(transferCommandBuffer, mStagingBuffer, mHandle, 1, &region);

    vkEndCommandBuffer(transferCommandBuffer);

    VkSubmitInfo submitInfo = {VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &transferCommandBuffer;

    vkQueueSubmit(getQueues().transfer, 1, &submitInfo, VK_NULL_HANDLE);
}

void VertexBuffer::SetData(VkDeviceSize size, void *data)
{
    StageData(size, data);
    PushData(size);
}

const VkBuffer VertexBuffer::GetHandle() const
{
    return mHandle;
}
