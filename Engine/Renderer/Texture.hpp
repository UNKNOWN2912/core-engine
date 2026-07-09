#pragma once
#include "Renderer/Sampler.hpp"
#include "Renderer/Utility.hpp"
#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

class Texture
{
public:
    void Create(void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter = Filter::Nearest, Filter magFilter = Filter::Nearest, AddressMode addressMode = AddressMode::Repeat);
    void Destroy();

    void Load(std::string_view filename, ImageFormat format = ImageFormat::RGBA8, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Repeat);
    const ImageDeprecated &GetImage() const;
    ImageDeprecated &GetImageRef();
    bool IsValid() const;
    const std::string &GetName() const;
    void SetName(const std::string &name);
    const Sampler &GetSampler() const;

private:
    std::string mName;
    bool mIsValid = false;
    ImageDeprecated mImage;
    Buffer mStagingBuffer;

    Sampler mSampler;
};