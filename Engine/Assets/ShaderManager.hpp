#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

enum class VertexShaderID : uint64_t;
enum class FragmentShaderID : uint64_t;
enum class GeometryShaderID : uint64_t;

class ShaderManager
{
public:
    static VertexShaderID LoadVertexShader(std::string_view filename);
    static VertexShaderID CreateVertexShader(const std::vector<uint32_t> &code);
    static VkShaderModule GetVertexShader(VertexShaderID id);
    static bool HasVertexShader(VertexShaderID id);
    static VertexShaderID GenerateVertexShaderID();
    static VertexShaderID GetInvalidVertexShaderID();

    static FragmentShaderID LoadFragmentShader(std::string_view filename);
    static FragmentShaderID CreateFragmentShader(const std::vector<uint32_t> &code);
    static VkShaderModule GetFragmentShader(FragmentShaderID id);
    static bool HasFragmentShader(FragmentShaderID id);
    static FragmentShaderID GenerateFragmentShaderID();
    static FragmentShaderID GetInvalidFragmentShaderID();

    static GeometryShaderID LoadGeometryShader(std::string_view filename);
    static GeometryShaderID CreateGeometryShader(const std::vector<uint32_t> &code);
    static VkShaderModule GetGeometryShader(GeometryShaderID id);
    static bool HasGeometryShader(GeometryShaderID id);
    static GeometryShaderID GenerateGeometryShaderID();
    static GeometryShaderID GetInvalidGeometryShaderID();

    static void Clear();

private:
    static uint64_t mLastVertexShaderId;
    static uint64_t mLastFragmentShaderId;
    static uint64_t mLastGeometryShaderId;
    static std::unordered_map<VertexShaderID, VkShaderModule> mVertexShaderMap;
    static std::unordered_map<FragmentShaderID, VkShaderModule> mFragmentShaderMap;
    static std::unordered_map<GeometryShaderID, VkShaderModule> mGeometryShaderMap;
};