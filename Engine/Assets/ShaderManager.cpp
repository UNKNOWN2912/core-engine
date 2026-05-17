#include "ShaderManager.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Utility.hpp"

VkShaderModule ShaderManager::LoadVertexShader(std::string_view filename)
{
    return LoadVertexShader(filename, filename);
}
VkShaderModule ShaderManager::LoadFragmentShader(std::string_view filename)
{
    return LoadFragmentShader(filename, filename);
}
VkShaderModule ShaderManager::LoadVertexShader(std::string_view filename, std::string_view identifier)
{
    mVertexShaderMap[identifier.data()] = CreateShaderFromFile(getDevice(), filename.data());
    return mVertexShaderMap[identifier.data()];
}
VkShaderModule ShaderManager::LoadFragmentShader(std::string_view filename, std::string_view identifier)
{
    mFragmentShaderMap[identifier.data()] = CreateShaderFromFile(getDevice(), filename.data());
    return mFragmentShaderMap[identifier.data()];
}
VkShaderModule ShaderManager::CreateVertexShader(std::string_view identifier, const std::vector<uint32_t>& code)
{
    mVertexShaderMap[identifier.data()] = CreateShaderModuleFromMemory(getDevice(), code);
    return mVertexShaderMap[identifier.data()];
}
VkShaderModule ShaderManager::CreateFragmentShader(std::string_view identifier, const std::vector<uint32_t>& code)
{
    mFragmentShaderMap[identifier.data()] = CreateShaderModuleFromMemory(getDevice(), code);
    return mFragmentShaderMap[identifier.data()];
}

VkShaderModule ShaderManager::GetVertexShader(std::string_view identifier)
{
    return mVertexShaderMap[identifier.data()];
}
VkShaderModule ShaderManager::GetFragmentShader(std::string_view identifier)
{
    return mFragmentShaderMap[identifier.data()];
}
bool ShaderManager::HasVertexShader(std::string_view identifier)
{
    return GetVertexShader(identifier) == nullptr;
}
bool ShaderManager::HasFragmentShader(std::string_view identifier)
{
    return GetFragmentShader(identifier) == nullptr;
}