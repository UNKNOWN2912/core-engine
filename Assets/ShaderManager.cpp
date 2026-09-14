#include "ShaderManager.hpp"
#include "Core/Application.hpp"
#include "Renderer/Renderer.hpp"

std::string ShaderManager::Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile, std::string_view tessellationFile, const ShaderConfig &config)
{
    Renderer &renderer = Application::GetInstance()->GetRenderer();
    mShaderMap[identifier.data()] = Shader(vertexFile, fragmentFile, geometryFile, tessellationFile, renderer.GetRenderPass(), renderer.GetRenderPassColorSubpassIndex(), config);
    return identifier.data();
}
std::string ShaderManager::Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, const ShaderConfig &config)
{
    return Load(identifier, vertexFile, fragmentFile, "", "", config);
}
std::string ShaderManager::Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, const ShaderConfig &config)
{
    Renderer &renderer = Application::GetInstance()->GetRenderer();
    mShaderMap[identifier.data()] = Shader(vertexCode, fragmentCode, geometryCode, tessellationCode, renderer.GetRenderPass(), renderer.GetRenderPassColorSubpassIndex(), config);
    return identifier.data();
}
std::string ShaderManager::Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const ShaderConfig &config)
{
    return Create(identifier, vertexCode, fragmentCode, {}, {}, config);
}

Shader &ShaderManager::Get(std::string_view identifier)
{
    return mShaderMap.at(identifier.data());
}

const Shader &ShaderManager::Get(std::string_view identifier) const
{
    return mShaderMap.at(identifier.data());
}

bool ShaderManager::Has(std::string_view identifier)
{
    return mShaderMap.contains(identifier.data());
}

const std::unordered_map<std::string, Shader> &ShaderManager::GetMap() const
{
    return ShaderManager::mShaderMap;
}
const BuiltinShaderIdentifier &ShaderManager::GetBuiltinIdentifier()
{
    return mBuiltinShaderIdentifier;
}

// uint64_t ShaderManager::mLastShaderId = 0;
// std::unordered_map<std::string, Shader> ShaderManager::mShaderMap;
// BuiltinShaderIdentifier ShaderManager::mBuiltinShaderIdentifier;