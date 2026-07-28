#include "ShaderManager.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Utility.hpp"

ShaderID ShaderManager::Load(std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile, std::string_view tessellationFile, bool createRendererObjects)
{
    ShaderID id = GenerateID();
    mShaderMap[id].vertex = CreateShaderFromFile(GraphicsContext::GetDevice(), vertexFile.data());
    mShaderMap[id].fragment = CreateShaderFromFile(GraphicsContext::GetDevice(), fragmentFile.data());
    mShaderMap[id].geometry = geometryFile.empty() ? VK_NULL_HANDLE : CreateShaderFromFile(GraphicsContext::GetDevice(), geometryFile.data());
    mShaderMap[id].tessellation = tessellationFile.empty() ? VK_NULL_HANDLE : CreateShaderFromFile(GraphicsContext::GetDevice(), tessellationFile.data());
    mShaderMap[id].vertexPath = vertexFile;
    mShaderMap[id].fragmentPath = fragmentFile;
    mShaderMap[id].geometryPath = geometryFile;
    mShaderMap[id].tessellationPath = tessellationFile;
    mShaderMap[id].createRendererObjects = createRendererObjects;
    if (createRendererObjects)
    {
        Renderer::CreateGraphicsPipeline(id);
    }
    return id;
}
ShaderID ShaderManager::Load(std::string_view vertexFile, std::string_view fragmentFile, bool createRendererObjects)
{
    return Load(vertexFile, fragmentFile, "", "", createRendererObjects);
}
ShaderID ShaderManager::Create(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, bool createRendererObjects)
{
    ShaderID id = GenerateID();
    mShaderMap[id].vertex = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), vertexCode);
    mShaderMap[id].fragment = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), fragmentCode);
    mShaderMap[id].geometry = geometryCode.empty() ? VK_NULL_HANDLE : CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), geometryCode);
    mShaderMap[id].tessellation = tessellationCode.empty() ? VK_NULL_HANDLE : CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), tessellationCode);
    mShaderMap[id].createRendererObjects = false;

    if (createRendererObjects)
    {
        Renderer::CreateGraphicsPipeline(id);
    }
    return id;
}
Shader &ShaderManager::Get(ShaderID id)
{
    return mShaderMap[id];
}
bool ShaderManager::Has(ShaderID id)
{
    return mShaderMap.contains(id);
}
ShaderID ShaderManager::GenerateID()
{
    return (ShaderID)mLastShaderId++;
}
ShaderID ShaderManager::GetInvalidID()
{
    return (ShaderID)UINT64_MAX;
}

const std::unordered_map<ShaderID, Shader> &ShaderManager::GetMap()
{
    return ShaderManager::mShaderMap;
}

uint64_t ShaderManager::mLastShaderId = 0;
std::unordered_map<ShaderID, Shader> ShaderManager::mShaderMap;