#pragma once
#include "Utility.hpp"
#include <vulkan/vulkan.h>

class UniformBuffer
{
public:
    UniformBuffer() = default;
    ~UniformBuffer();
    UniformBuffer(const UniformBuffer &) = delete;
    UniformBuffer &operator=(const UniformBuffer &) = delete;
    UniformBuffer(size_t size, void *data = nullptr);
    UniformBuffer(UniformBuffer &&uniformBuffer) noexcept;
    UniformBuffer &operator=(UniformBuffer &&uniformBuffer) noexcept;

    void SetData(void *data);
    const Buffer &GetBuffer() const;
    size_t GetCapacity();

private:
    void Destroy();
    Buffer mBuffer;
};