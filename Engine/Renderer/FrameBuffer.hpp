#pragma once
#include "Renderer/Image.hpp"
#include <cstdint>

class FrameBuffer
{
    public:
        uint64_t GetId() { return mId; }
    private:
        friend class Graphic;
        uint64_t mId = 0;
        std::vector<DeviceImage> mAttachments;
};