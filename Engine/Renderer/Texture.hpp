#pragma once
#include "Renderer/Utility.hpp"
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class Texture
{
public:
    void Create(void *data, const glm::uvec2 &size, ImageFormat format);
    void Destroy();

    void Load(std::string_view filename);
    const ImageDeprecated &GetImage() const
    {
        return mImage;
    }
    ImageDeprecated &GetImageRef()
    {
        return mImage;
    }
    bool IsValid() const
    {
        return mIsValid;
    }

    ~Texture();

    const std::string &GetName() const
    {
        return mName;
    }

    void SetName(const std::string &name)
    {
        mName = name;
    }

private:
    std::string mName;
    bool mIsValid = false;
    ImageDeprecated mImage;
    Buffer mStagingBuffer;
};