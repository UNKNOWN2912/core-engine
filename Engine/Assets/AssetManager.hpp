#pragma once
#include <memory>
#include <string>
#include <unordered_map>

struct Shader
{
    std::string vertexCode;
    std::string fragmentCode;

    Shader(std::string_view vertexCode, std::string_view fragmentCode): vertexCode(vertexCode), fragmentCode(fragmentCode) {}
    Shader(){}

    bool operator==(const Shader& shader)
    {
        return (this->vertexCode == shader.vertexCode && this->fragmentCode == fragmentCode);
    }
};

class AssetManager
{
    public:
        std::shared_ptr<Shader> LoadShader(std::string_view identifier, std::string_view vertexShader, std::string_view fragmentShader);    
        void UnloadShader(std::string_view identifier);

        const std::unordered_map<std::string, std::shared_ptr<Shader>>& GetShaderMap() { return mShaders; }

    private:
        std::unordered_map<std::string, std::shared_ptr<Shader>> mShaders;
    
};

