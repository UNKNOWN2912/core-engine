#include "TextureManager.hpp"

TextureID TextureManager::GenerateID()
{
    return (TextureID)mLastTextureId++;
}

TextureID TextureManager::LoadTexture(std::string_view filename)
{
    TextureID id = GenerateID();

    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Load(filename);

    mTextureMap[id] = texture;

    return id;
}

TextureID TextureManager::CreateTexture(void *data, const glm::uvec2 &size, ImageFormat format)
{
    TextureID id = GenerateID();

    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Create(data, size, format);

    mTextureMap[id] = texture;

    return id;
}

void TextureManager::DestroyTexture(TextureID id)
{
    mTextureMap[id].reset();
}

std::shared_ptr<Texture> TextureManager::GetTexture(TextureID id)
{
    return mTextureMap[id];
}

bool TextureManager::HasTexture(TextureID id)
{
    return mTextureMap.contains(id);
}

uint32_t TextureManager::GetCount()
{
    return mTextureMap.size();
}

uint64_t TextureManager::mLastTextureId = 0;
std::unordered_map<TextureID, std::shared_ptr<Texture>> TextureManager::mTextureMap;
