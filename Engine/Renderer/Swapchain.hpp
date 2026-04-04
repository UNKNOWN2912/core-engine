#pragma once
#include "Renderer/Image.hpp"
#include <cstdint>


class Swapchain
{
    public:
        uint64_t GetId() const { return mId; }
        const std::vector<DeviceImage>& GetImages() const { return mImages; }
        ImageFormat GetFormat() const { return mFormat; }
        const glm::uvec2& GetSize() const { return mSize; }

    private:
        friend class Graphic;
        uint64_t mId = 0;
        std::vector<DeviceImage> mImages;
        ImageFormat mFormat = ImageFormat::BGRA8;
        glm::uvec2 mSize = {800,600};
};