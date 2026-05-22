#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
public:
    UniformBuffer() = default;
    ~UniformBuffer();
    UniformBuffer(const UniformBuffer &) = default;
    UniformBuffer &operator=(const UniformBuffer &) = default;
    UniformBuffer(size_t size);
    UniformBuffer(size_t size, void *data);
    UniformBuffer(UniformBuffer &&uniformBuffer) noexcept;
    UniformBuffer &operator=(UniformBuffer &&uniformBuffer) noexcept;

    void Destroy();
    void SetData(void *data);
    const Buffer &GetBuffer() const;
    size_t GetCapacity();

private:
    Buffer mBuffer;
};