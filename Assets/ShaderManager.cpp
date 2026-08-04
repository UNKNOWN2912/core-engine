#include "ShaderManager.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Utility.hpp"

std::string ShaderManager::Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile, std::string_view tessellationFile, bool createRendererObjects)
{
    mShaderMap[identifier.data()].vertex = CreateShaderFromFile(GraphicsContext::GetDevice(), vertexFile.data());
    mShaderMap[identifier.data()].fragment = CreateShaderFromFile(GraphicsContext::GetDevice(), fragmentFile.data());
    mShaderMap[identifier.data()].geometry = geometryFile.empty() ? VK_NULL_HANDLE : CreateShaderFromFile(GraphicsContext::GetDevice(), geometryFile.data());
    mShaderMap[identifier.data()].tessellation = tessellationFile.empty() ? VK_NULL_HANDLE : CreateShaderFromFile(GraphicsContext::GetDevice(), tessellationFile.data());
    mShaderMap[identifier.data()].vertexPath = vertexFile;
    mShaderMap[identifier.data()].fragmentPath = fragmentFile;
    mShaderMap[identifier.data()].geometryPath = geometryFile;
    mShaderMap[identifier.data()].tessellationPath = tessellationFile;
    mShaderMap[identifier.data()].createRendererObjects = createRendererObjects;
    if (createRendererObjects)
    {
        Renderer::CreateGraphicsPipeline(identifier);
    }
    return identifier.data();
}
std::string ShaderManager::Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, bool createRendererObjects)
{
    return Load(identifier, vertexFile, fragmentFile, "", "", createRendererObjects);
}
std::string ShaderManager::Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, bool createRendererObjects)
{
    mShaderMap[identifier.data()].vertex = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), vertexCode);
    mShaderMap[identifier.data()].fragment = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), fragmentCode);
    mShaderMap[identifier.data()].geometry = geometryCode.empty() ? VK_NULL_HANDLE : CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), geometryCode);
    mShaderMap[identifier.data()].tessellation = tessellationCode.empty() ? VK_NULL_HANDLE : CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), tessellationCode);
    mShaderMap[identifier.data()].createRendererObjects = false;

    if (createRendererObjects)
    {
        Renderer::CreateGraphicsPipeline(identifier);
    }
    return identifier.data();
}
Shader &ShaderManager::Get(std::string_view identifier)
{
    return mShaderMap[identifier.data()];
}
bool ShaderManager::Has(std::string_view identifier)
{
    return mShaderMap.contains(identifier.data());
}

const std::unordered_map<std::string, Shader> &ShaderManager::GetMap()
{
    return ShaderManager::mShaderMap;
}

uint64_t ShaderManager::mLastShaderId = 0;
std::unordered_map<std::string, Shader> ShaderManager::mShaderMap;
BuiltinShaderIdentifier ShaderManager::mBuiltinShaderIdentifier;