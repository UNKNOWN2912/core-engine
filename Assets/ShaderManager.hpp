#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

struct BuiltinShaderIdentifier
{
    std::string_view pbr = "builtinPhysical";
    std::string_view text = "builtinText";
    std::string_view directionalShadow = "builtinDirectionalShadow";
    std::string_view pointShadow = "builtinPointShadow";
};

struct Shader
{
    VkShaderModule vertex;
    VkShaderModule fragment;
    VkShaderModule geometry;
    VkShaderModule tessellation;

    std::string vertexPath;
    std::string fragmentPath;
    std::string geometryPath;
    std::string tessellationPath;

    bool createRendererObjects;
};

class ShaderManager
{
public:
    static std::string Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile, std::string_view tessellationFile, bool createRendererObjects = true);
    static std::string Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, bool createRendererObjects = true);
    static std::string Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode = {}, const std::vector<uint32_t> &tessellationCode = {}, bool createRendererObjects = true);
    static Shader &Get(std::string_view id);
    static bool Has(std::string_view id);
    static const std::unordered_map<std::string, Shader> &GetMap();
    static const BuiltinShaderIdentifier &GetBuiltinIdentifier()
    {
        return mBuiltinShaderIdentifier;
    }

private:
    static uint64_t mLastShaderId;
    static std::unordered_map<std::string, Shader> mShaderMap;
    static BuiltinShaderIdentifier mBuiltinShaderIdentifier;
};