#pragma once
#include <cstdint>

class DeviceSemaphore
{
    public:
        uint64_t GetId() { return mId; }
    private:
        friend class Graphic;
        uint64_t mId = 0;
};