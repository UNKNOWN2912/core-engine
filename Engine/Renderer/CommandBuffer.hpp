#pragma once
#include <cstdint>

class CommandBuffer
{
    public:
        uint64_t GetId() { return mId; }

    private:
        friend class Graphic;
        uint64_t mId = UINT64_MAX;
};