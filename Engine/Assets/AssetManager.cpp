#include "AssetManager.hpp"
#include "Core/Macro.hpp"
#include <cstdio>
#include <shaderc/shaderc.h>


std::shared_ptr<Shader> AssetManager::LoadShader(std::string_view identifier, std::string_view vertexShader, std::string_view fragmentShader) 
{
    std::shared_ptr<Shader> shader;
    shader.reset(new Shader());

    FILE* fp = fopen(vertexShader.data(), "r");
    if(fp == nullptr) { ERROR("File not found: {}", vertexShader); }
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    shader->vertexCode.resize(size);
    fread(shader->vertexCode.data(), size, 1, fp);
    fclose(fp);

    fp = fopen(fragmentShader.data(), "r");
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    shader->fragmentCode.resize(size);
    fread(shader->fragmentCode.data(), size, 1, fp);
    fclose(fp);

    mShaders[identifier.data()] = shader;

    return shader;
};

void AssetManager::UnloadShader(std::string_view identifier) 
{
    mShaders.erase(mShaders.find(identifier.data()));    
}
