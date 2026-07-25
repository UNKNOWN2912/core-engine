#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

enum class ShaderID : uint64_t;
#define INVALID_SHADER_ID ShaderID(UINT64_MAX);

struct Shader
{
    VkShaderModule vertex;
    VkShaderModule fragment;
    VkShaderModule geometry;
    VkShaderModule tessellation;
};

class ShaderManager
{
public:
    static ShaderID Load(std::string_view vertexFile, std::string_view fragmentFile, std::string_view geometryFile = "", std::string_view tessellationFile = "", bool createRendererObjects = true);
    static ShaderID Load(std::string_view vertexFile, std::string_view fragmentFile, bool createRendererObjects);
    static ShaderID Create(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode = {}, const std::vector<uint32_t> &tessellationCode = {}, bool createRendererObjects = true);
    static Shader &Get(ShaderID id);
    static bool Has(ShaderID id);
    static ShaderID GenerateID();
    static ShaderID GetInvalidID();

private:
    static uint64_t mLastShaderId;
    static std::unordered_map<ShaderID, Shader> mShaderMap;
};
