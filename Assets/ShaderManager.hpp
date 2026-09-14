#pragma once
#include "Renderer/Shader.hpp"
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

class ShaderManager
{
public:
    std::string Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile, std::string_view tessellationFile, const ShaderConfig &config);
    std::string Load(std::string_view identifier, std::string_view vertexFile, std::string_view fragmentFile, const ShaderConfig &config);
    std::string Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, const ShaderConfig &config);
    std::string Create(std::string_view identifier, const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const ShaderConfig &config);
    Shader &Get(std::string_view id);
    const Shader &Get(std::string_view id) const;
    bool Has(std::string_view id);
    const std::unordered_map<std::string, Shader> &GetMap() const;
    const BuiltinShaderIdentifier &GetBuiltinIdentifier();

private:
    uint64_t mLastShaderId;
    std::unordered_map<std::string, Shader> mShaderMap;
    BuiltinShaderIdentifier mBuiltinShaderIdentifier;
};