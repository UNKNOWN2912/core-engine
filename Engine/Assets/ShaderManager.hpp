#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

enum class VertexShaderID : uint64_t;
enum class FragmentShaderID : uint64_t;

class ShaderManager
{
  public:
    static VertexShaderID LoadVertexShader(std::string_view filename);
    static FragmentShaderID LoadFragmentShader(std::string_view filename);

    static VertexShaderID CreateVertexShader(const std::vector<uint32_t> &code);
    static FragmentShaderID CreateFragmentShader(const std::vector<uint32_t> &code);

    static VkShaderModule GetVertexShader(VertexShaderID id);
    static VkShaderModule GetFragmentShader(FragmentShaderID id);

    static bool HasVertexShader(VertexShaderID id);
    static bool HasFragmentShader(FragmentShaderID id);

    static VertexShaderID GenerateVertexShaderID();
    static FragmentShaderID GenerateFragmentShaderID();

  private:
    static uint64_t mLastVertexShaderId;
    static uint64_t mLastFragmentShaderId;
    static std::unordered_map<VertexShaderID, VkShaderModule> mVertexShaderMap;
    static std::unordered_map<FragmentShaderID, VkShaderModule> mFragmentShaderMap;
};