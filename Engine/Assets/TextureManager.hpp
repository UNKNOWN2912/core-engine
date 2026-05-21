#pragma once
#include "Renderer/Texture.hpp"
#include <memory>
#include <unordered_map>

enum class TextureID : uint64_t;

class TextureManager
{
  public:
    static TextureID LoadTexture(std::string_view filename);
    static TextureID CreateTexture(void *data, const glm::uvec2 &size, ImageFormat format);

    static TextureID GenerateID();

    static void DestroyTexture(TextureID textureId);

    static std::shared_ptr<Texture> GetTexture(TextureID textureId);
    static bool HasTexture(TextureID textureId);

    static uint32_t GetCount();

    static const std::unordered_map<TextureID, std::shared_ptr<Texture>> &GetMap()
    {
        return mTextureMap;
    }

    static constexpr TextureID GetInvalidID()
    {
        return (TextureID)UINT64_MAX;
    }

  private:
    static uint64_t mLastTextureId;
    static std::unordered_map<TextureID, std::shared_ptr<Texture>> mTextureMap;
};