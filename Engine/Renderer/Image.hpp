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

enum class ImageUsage : uint8_t
{
    None = 0,
    ColorOutput = 1,
    Sample = 2,
    Storage = 4,
    TransferSrc = 8,
    TransferDst = 16,
    SubpassInput = 32,
    Depth = 64,
    Present = 128,
};

inline ImageUsage operator|(ImageUsage lhs, ImageUsage rhs)
{
    return ImageUsage(uint8_t(lhs) | uint8_t(rhs));
}

inline ImageUsage operator&(ImageUsage lhs, ImageUsage rhs)
{
    return ImageUsage(uint8_t(lhs) & uint8_t(rhs));
}


class DeviceImage
{
    public:
        uint32_t GetId() const { return mId; }
        ImageFormat GetFormat() const { return mFormat; }
        const glm::uvec2& GetSize() const { return mSize; }
        ImageUsage GetImageUsage() const { return mUsage; }

    private:
        friend class Graphic;
        uint64_t mId = 0;
        glm::uvec2 mSize = {0,0};
        ImageFormat mFormat = ImageFormat::None;
        ImageUsage mUsage = ImageUsage::None;
};