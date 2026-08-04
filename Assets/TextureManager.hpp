#pragma once
#include "Renderer/Descriptor.hpp"
#include "Renderer/Texture.hpp"
#include <memory>
#include <unordered_map>

class TextureManager
{
public:
    static void Initialize();
    static void Terminate();
    static std::string LoadTexture(std::string_view identifier, std::string_view filename, ImageFormat format = ImageFormat::RGBA8);
    static std::string CreateTexture(std::string_view identifier, void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Border);

    static void DestroyTexture(std::string_view identifier);

    static const Texture &GetTexture(std::string_view identifier);
    static Texture &GetTextureRef(std::string_view identifier);
    static bool HasTexture(std::string_view identifier);

    static uint32_t GetCount();

    static const std::unordered_map<std::string, Texture> &GetMap();

    static const Descriptor &GetDescriptor();

    static uint32_t GetTextureDescriptorIndex(std::string_view identifier);

    static void Clear();

private:
    static Sampler mSampler;
    static Descriptor mDescriptor;
    static std::unordered_map<std::string, Texture> mTextureMap;
    static std::unordered_map<std::string, uint32_t> mTextureDescriptorIndex;
};