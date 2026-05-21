#include "ShaderManager.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Utility.hpp"

VertexShaderID ShaderManager::LoadVertexShader(std::string_view filename)
{
    VertexShaderID id = GenerateVertexShaderID();
    mVertexShaderMap[id] = CreateShaderFromFile(getDevice(), filename.data());
    return id;
}
FragmentShaderID ShaderManager::LoadFragmentShader(std::string_view filename)
{
    FragmentShaderID id = GenerateFragmentShaderID();
    mFragmentShaderMap[id] = CreateShaderFromFile(getDevice(), filename.data());
    return id;
}
VertexShaderID ShaderManager::CreateVertexShader(const std::vector<uint32_t> &code)
{
    VertexShaderID id = GenerateVertexShaderID();
    mVertexShaderMap[id] = CreateShaderModuleFromMemory(getDevice(), code);
    return id;
}

FragmentShaderID ShaderManager::CreateFragmentShader(const std::vector<uint32_t> &code)
{
    FragmentShaderID id = GenerateFragmentShaderID();
    mFragmentShaderMap[id] = CreateShaderModuleFromMemory(getDevice(), code);
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

uint64_t ShaderManager::mLastVertexShaderId = 0;
uint64_t ShaderManager::mLastFragmentShaderId = 0;
std::unordered_map<VertexShaderID, VkShaderModule> ShaderManager::mVertexShaderMap;
std::unordered_map<FragmentShaderID, VkShaderModule> ShaderManager::mFragmentShaderMap;
