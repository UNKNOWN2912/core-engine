#include "TextureManager.hpp"

std::shared_ptr<Texture> TextureManager::LoadTexture(std::string_view filename) 
{
    return LoadTexture(filename, filename);
}

std::shared_ptr<Texture> TextureManager::LoadTexture(std::string_view filename, std::string_view identifier) 
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Load(filename);

    mTextureMap[identifier.data()] = texture;

    return texture;
}

std::shared_ptr<Texture> TextureManager::CreateTexture(std::string_view identifier, void* data, const glm::uvec2& size, ImageFormat format) 
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>();
    texture->Create(data, size, format);

    mTextureMap[identifier.data()] = texture;

    return texture;
}

void TextureManager::DestroyTexture(std::string_view identifier) 
{
    mTextureMap[identifier.data()].reset();    
}

std::shared_ptr<Texture> TextureManager::GetTexture(std::string_view identifier) 
{
    return mTextureMap[identifier.data()];
}

bool TextureManager::HasTexture(std::string_view identifier) 
{
    return GetTexture(identifier) == nullptr;
}
