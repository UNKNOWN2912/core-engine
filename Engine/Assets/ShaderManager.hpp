#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

class ShaderManager
{
    public:
        VkShaderModule LoadVertexShader(std::string_view filename);
        VkShaderModule LoadFragmentShader(std::string_view filename);

        VkShaderModule LoadVertexShader(std::string_view filename, std::string_view identifier);
        VkShaderModule LoadFragmentShader(std::string_view filename, std::string_view identifier);

        VkShaderModule CreateVertexShader(std::string_view identifier, const std::vector<uint32_t>& code);
        VkShaderModule CreateFragmentShader(std::string_view identifier, const std::vector<uint32_t>& code);

        VkShaderModule GetVertexShader(std::string_view identifier);
        VkShaderModule GetFragmentShader(std::string_view identifier);

        bool HasVertexShader(std::string_view identifier);
        bool HasFragmentShader(std::string_view identifier);
    private:
        std::unordered_map<std::string, VkShaderModule> mVertexShaderMap;
        std::unordered_map<std::string, VkShaderModule> mFragmentShaderMap;
};