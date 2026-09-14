#pragma once
#include "Renderer/GraphicsPipeline.hpp"
#include "Vertex.hpp"
#include <string_view>
#include <vector>
#include <vulkan/vulkan_core.h>

struct ShaderConfig
{
    SampleCount sampleCount = SampleCount::One;
    CullMode cullMode = CullMode::Back;
    bool enableDepthWrite = false;
    bool enableDepthTest = false;
    PrimitiveType primitive = PrimitiveType::Triangle;
    CompareType compare = CompareType::Less;

    std::vector<std::reference_wrapper<const Descriptor>> descriptors;
    std::vector<bool> colorBlendAttachments;
    std::vector<VertexLayout> layouts;
    size_t pushConstantSize = 0;
    bool enableDepthBias = false;
    float slopeFactor = 0.f;
    float constantFactor = 0.f;
};

class Shader
{
public:
    const ShaderConfig &GetSettings() const;
    ShaderConfig &GetSettings();
    const GraphicsPipeline &GetGraphicsPipeline() const;

    const std::string &GetVertexFilename() const;
    const std::string &GetFragmentFilename() const;
    const std::string &GetGeometryFilename() const;
    const std::string &GetTessellationFilename() const;

    Shader() = default;
    Shader(std::string_view vertexFilename, std::string_view fragmentFilename, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings)
    {
        Load(vertexFilename, fragmentFilename, renderPass, subpass, settings);
    }
    Shader(std::string_view vertexFilename, std::string_view fragmentFilename, std::string_view geometryFilename, std::string_view tesellationFilename, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings)
    {
        Load(vertexFilename, fragmentFilename, geometryFilename, tesellationFilename, renderPass, subpass, settings);
    }
    Shader(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings)
    {
        Create(vertexCode, fragmentCode, renderPass, subpass, settings);
    }
    Shader(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings);

    Shader(Shader &&shader) noexcept;
    Shader &operator=(Shader &&shader) noexcept;
    ~Shader();

    Shader(const Shader &shader) = delete;
    Shader &operator=(const Shader &shader) = delete;

private:
    void AddDescriptor(const Descriptor &descriptor);
    void AddColorBlendAttachment(bool enableBlending);
    void AddLayout(const VertexLayout &layout);
    void SetPushConstantSize(size_t size);
    void SetDepthBias(bool enable, float slopeFactor, float constantFactor);
    void SetupPipelineSettings(const ShaderConfig &settings);

    void Load(std::string_view vertexFilename, std::string_view fragmentFilename, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings);
    void Load(std::string_view vertexFilename, std::string_view fragmentFilename, std::string_view geometryFilename, std::string_view tesellationFilename, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings);
    void Create(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings);
    void Create(const std::vector<uint32_t> &vertexCode, const std::vector<uint32_t> &fragmentCode, const std::vector<uint32_t> &geometryCode, const std::vector<uint32_t> &tessellationCode, const RenderPass &renderPass, uint32_t subpass, const ShaderConfig &settings);
    void Destroy();

    GraphicsPipeline mGraphicsPipeline;
    ShaderConfig mSettings;

    VkShaderModule mVertexShaderModule = VK_NULL_HANDLE;
    VkShaderModule mFragmentShaderModule = VK_NULL_HANDLE;
    VkShaderModule mGeometryShaderModule = VK_NULL_HANDLE;
    VkShaderModule mTessellationShaderModule = VK_NULL_HANDLE;

    std::string mVertexFilename;
    std::string mFragmentFilename;
    std::string mGeometryFilename;
    std::string mTessellationFilename;

    std::vector<std::reference_wrapper<const Descriptor>> mDescriptors;
};
