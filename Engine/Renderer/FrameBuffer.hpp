#pragma once
#include "Renderer/Image.hpp"
#include <cstdint>

class FrameBuffer
{
    public:
        uint64_t GetId() const { return mId; }
        FrameBuffer& SetId(uint64_t id) { mId = id; return *this; }
        FrameBuffer& AddAttachment(const DeviceImage& image) { mAttachments.push_back(image); return *this; }

        const std::vector<DeviceImage>& GetAttachments() const { return mAttachments; }



    private:
        uint64_t mId = 0;
        std::vector<DeviceImage> mAttachments;
};