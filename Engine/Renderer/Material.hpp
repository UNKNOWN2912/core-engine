#pragma once
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Texture.hpp"

enum class CullMode
{
    None = 0,
    Front,
    Back,
};

enum class FrontFace
{
    None = 0,
    Clockwise, 
    CounterClockwise,
};

enum class PrimitiveType
{
    None = 0,
    Triangle, 
    Line, 
    Point,
};

struct MaterialSettings
{
    bool depthTestEnable = true;
    bool depthWriteEnable = true;
    bool enableInstancing = false;
    bool wireframe = false;
    bool blendEnable = true;
    CullMode cullMode = CullMode::Back;
    PrimitiveType primitiveType = PrimitiveType::Triangle;
    FrontFace frontFace = FrontFace::Clockwise;
    float lineWidth = 1.f;
    int sampleCount = 1; 
};

class Material
{
    public:
        void LoadAlbedo(std::string_view filename);
        void LoadShaders(std::string_view vertexShader, std::string_view fragmentShader);

        void Create();

        MaterialSettings& GetSettingsRef();
    private:
        friend class Renderer;

        Texture mAlbedo;
        GraphicsPipeline mPipeline;
        MaterialSettings mSettings;

        VkDescriptorSet mDescriptorSet;
        VkDescriptorSetLayout mSetLayout;

        VkPipelineLayout mPipelineLayout;

        VkDescriptorPool mDescriptorPool;
};