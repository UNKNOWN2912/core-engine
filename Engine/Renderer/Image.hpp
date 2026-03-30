#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <string_view>
#include "Enums.hpp"

using byte = unsigned char;

class HostImage
{
    public:
        ImageFormat GetFormat() const { return mFormat; }
        const glm::uvec2& GetSize() const { return mSize; }
        void* GetData() const { return mData; }
        
        void CopyData(byte* data, const glm::uvec2& size, ImageFormat format);
        void SetData(byte* data, const glm::uvec2& size, ImageFormat format);

        void LoadFromFile(std::string_view filename);
        
    private:
        byte* mData = nullptr;
        glm::uvec2 mSize = {0,0};
        ImageFormat mFormat = ImageFormat::None;
};

class DeviceImage
{
    public:
        uint32_t GetId() const { return mId; }
        ImageFormat GetFormat() const { return mFormat; }
        const glm::uvec2& GetSize() const { return mSize; }

    private:
        uint32_t mId = 0;
        glm::uvec2 mSize = {0,0};
        ImageFormat mFormat = ImageFormat::None;
};