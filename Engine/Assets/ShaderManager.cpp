#include "ShaderManager.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Utility.hpp"

VertexShaderID ShaderManager::LoadVertexShader(std::string_view filename)
{
    VertexShaderID id = GenerateVertexShaderID();
    mVertexShaderMap[id] = CreateShaderFromFile(GraphicsContext::GetDevice(), filename.data());
    return id;
}
FragmentShaderID ShaderManager::LoadFragmentShader(std::string_view filename)
{
    FragmentShaderID id = GenerateFragmentShaderID();
    mFragmentShaderMap[id] = CreateShaderFromFile(GraphicsContext::GetDevice(), filename.data());
    return id;
}
VertexShaderID ShaderManager::CreateVertexShader(const std::vector<uint32_t> &code)
{
    VertexShaderID id = GenerateVertexShaderID();
    mVertexShaderMap[id] = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), code);
    return id;
}

FragmentShaderID ShaderManager::CreateFragmentShader(const std::vector<uint32_t> &code)
{
    FragmentShaderID id = GenerateFragmentShaderID();
    mFragmentShaderMap[id] = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), code);
    return id;
}

VkShaderModule ShaderManager::GetVertexShader(VertexShaderID id)
{
    return mVertexShaderMap[id];
}
VkShaderModule ShaderManager::GetFragmentShader(FragmentShaderID id)
{
    return mFragmentShaderMap[id];
}
bool ShaderManager::HasVertexShader(VertexShaderID id)
{
    return mVertexShaderMap.contains(id);
}
bool ShaderManager::HasFragmentShader(FragmentShaderID id)
{
    return mFragmentShaderMap.contains(id);
}

VertexShaderID ShaderManager::GenerateVertexShaderID()
{
    return (VertexShaderID)mLastVertexShaderId++;
}

FragmentShaderID ShaderManager::GenerateFragmentShaderID()
{
    return (FragmentShaderID)mLastFragmentShaderId++;
}
VertexShaderID ShaderManager::GetInvalidVertexShaderID()
{
    return (VertexShaderID)UINT64_MAX;
}
FragmentShaderID ShaderManager::GetInvalidFragmentShaderID()
{
    return (FragmentShaderID)UINT64_MAX;
}

GeometryShaderID ShaderManager::LoadGeometryShader(std::string_view filename)
{
    GeometryShaderID id = GenerateGeometryShaderID();
    mGeometryShaderMap[id] = CreateShaderFromFile(GraphicsContext::GetDevice(), filename.data());
    return id;
}
GeometryShaderID ShaderManager::CreateGeometryShader(const std::vector<uint32_t> &code)
{
    GeometryShaderID id = GenerateGeometryShaderID();
    mGeometryShaderMap[id] = CreateShaderModuleFromMemory(GraphicsContext::GetDevice(), code);
    return id;
}
VkShaderModule ShaderManager::GetGeometryShader(GeometryShaderID id)
{
    return mGeometryShaderMap[id];
}
bool ShaderManager::HasGeometryShader(GeometryShaderID id)
{
    return mGeometryShaderMap.contains(id);
}
GeometryShaderID ShaderManager::GenerateGeometryShaderID()
{
    return (GeometryShaderID)mLastGeometryShaderId++;
}
GeometryShaderID ShaderManager::GetInvalidGeometryShaderID()
{
    return (GeometryShaderID)UINT32_MAX;
}

void ShaderManager::Clear()
{
    for (auto &[id, module] : mVertexShaderMap)
    {
        vkDestroyShaderModule(GraphicsContext::GetDevice(), module, nullptr);
    }
    for (auto &[id, module] : mFragmentShaderMap)
    {
        vkDestroyShaderModule(GraphicsContext::GetDevice(), module, nullptr);
    }
    mVertexShaderMap.clear();
    mFragmentShaderMap.clear();
}

uint64_t ShaderManager::mLastVertexShaderId = 0;
uint64_t ShaderManager::mLastFragmentShaderId = 0;
uint64_t ShaderManager::mLastGeometryShaderId = 0;
std::unordered_map<VertexShaderID, VkShaderModule> ShaderManager::mVertexShaderMap;
std::unordered_map<FragmentShaderID, VkShaderModule> ShaderManager::mFragmentShaderMap;
std::unordered_map<GeometryShaderID, VkShaderModule> ShaderManager::mGeometryShaderMap;
