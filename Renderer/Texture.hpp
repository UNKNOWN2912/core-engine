#pragma once
#include "Renderer/Image.hpp"
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
    Texture() = default;
    Texture(void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Repeat)
    {
        Create(data, size, format, minFilter, magFilter, addressMode);
    }
    Texture(std::string_view filename, ImageFormat format = ImageFormat::RGBA8, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Repeat)
    {
        Load(filename, format, minFilter, magFilter, AddressMode::Repeat);
    }
    ~Texture()
    {
        Destroy();
    }
    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;

    Texture(Texture &&texture) noexcept
    {
        mSampler = std::move(texture.mSampler);
        mImage = std::move(texture.mImage);
        mName = std::move(texture.mName);
        mFileName = std::move(texture.mFileName);
    }
    Texture &operator=(Texture &&texture) noexcept
    {
        Destroy();

        mSampler = std::move(texture.mSampler);
        mImage = std::move(texture.mImage);

        return *this;
    }

    void Create(void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Repeat);
    void Load(std::string_view filename, ImageFormat format = ImageFormat::RGBA8, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Repeat);
    void Destroy();

    const Image &GetImage() const;
    Image &GetImage();
    const std::string &GetName() const;
    void SetName(const std::string &name);
    const Sampler &GetSampler() const;

    void SetFilename(std::string_view filename);
    const std::string &GetFilename() const;

private:
    std::string mName = "Untitled";
    std::string mFileName;
    Image mImage;

    Sampler mSampler;
};