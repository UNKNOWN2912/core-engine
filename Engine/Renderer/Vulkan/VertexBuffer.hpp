#pragma once
#include <vulkan/vulkan.h>
#include <string.h>

class VertexBuffer
{
  public:
    void Create(VkDeviceSize capacity);
    void StageData(size_t size, void *data);
    void PushData(VkDeviceSize size);
    void SetData(VkDeviceSize size, void *data);

    const VkBuffer GetHandle() const;

  private:
    VkBuffer mStagingBuffer;
    VkDeviceMemory mStagingMemory;
    VkBuffer mHandle;
    VkDeviceMemory mMemory;
    void *mStageMemoryMap;

    VkDeviceSize mCapacity;
};