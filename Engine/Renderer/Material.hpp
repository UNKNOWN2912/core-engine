#pragma once
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Types.hpp"

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

enum class AttributeType
{
    Int, UInt, Float,
    IVec2, UVec2, Vec2,
    IVec3, UVec3, Vec3,
    IVec4, UVec4, Vec4,
};

enum class InputRate
{
    None = 0,
    Vertex,
    Instance
};


class Material
{
    public:
        Material();
        void LoadAlbedo(std::string_view filename);
        void LoadShaders(std::string_view vertexShader, std::string_view fragmentShader);

        void ClearBindingAttribute();
        void SetBindingAttribute(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> layout);

        void Create();

        MaterialSettings& GetSettingsRef();

        void SetAlbedoSampler(Filter mag, Filter min, std::array<AddressMode, 3> addressModes);

        bool IsValid() const { return mIsValid; }
    private:
        friend class Renderer;

        Texture mAlbedo;
        GraphicsPipeline mPipeline;
        MaterialSettings mSettings;

        VkDescriptorSet mDescriptorSet;
        VkDescriptorSetLayout mSetLayout;

        VkPipelineLayout mPipelineLayout;

        VkDescriptorPool mDescriptorPool;

        uint32_t mFinalLocation = 0;

        bool mIsValid = false;
};