#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include "Renderer/Texture.hpp"

class TextureManager
{
    public:
        std::shared_ptr<Texture> LoadTexture(std::string_view filename);
        std::shared_ptr<Texture> LoadTexture(std::string_view filename, std::string_view identifier);
        std::shared_ptr<Texture> CreateTexture(std::string_view identifier, void* data, const glm::uvec2& size, ImageFormat format);

        void DestroyTexture(std::string_view identifier);

        std::shared_ptr<Texture> GetTexture(std::string_view identifier);
        bool HasTexture(std::string_view identifer);


    private:
        std::unordered_map<std::string, std::shared_ptr<Texture>> mTextureMap;
};