#pragma once
#include "Renderer/Descriptor.hpp"
#include "Renderer/Texture.hpp"
#include <unordered_map>

class TextureManager
{
public:
    void Initialize();
    void Terminate();
    std::string LoadTexture(std::string_view identifier, std::string_view filename, ImageFormat format = ImageFormat::RGBA8);
    std::string CreateTexture(std::string_view identifier, void *data, const glm::uvec2 &size, ImageFormat format, Filter minFilter = Filter::Linear, Filter magFilter = Filter::Linear, AddressMode addressMode = AddressMode::Border);
    std::string MoveTexture(std::string_view identifier, Texture &&texture);

    void DestroyTexture(std::string_view identifier);

    const Texture &GetTexture(std::string_view identifier);
    Texture &GetTextureRef(std::string_view identifier);
    bool HasTexture(std::string_view identifier);

    uint32_t GetCount();

    const std::unordered_map<std::string, Texture> &GetMap() const;

    void SetTextureDescriptor(const Descriptor &descriptor);
    uint32_t GetTextureDescriptorIndex(std::string_view identifier) const;

    void Clear();

private:
    std::unordered_map<std::string, Texture> mTextureMap;
    std::unordered_map<std::string, uint32_t> mTextureDescriptorIndex;
};