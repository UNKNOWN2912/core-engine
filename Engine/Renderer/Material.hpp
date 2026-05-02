#pragma once
#include "Renderer/Descriptor.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Types.hpp"

enum class AttributeType
{
    Int, UInt, Float,
    IVec2, UVec2, Vec2,
    IVec3, UVec3, Vec3,
    IVec4, UVec4, Vec4,
};

class Material
{
    public:
        void LoadAlbedo(std::string_view filename);
        void LoadShaders(std::string_view vertexShaderFilename,
                        std::string_view fragmentShaderFilename);

        void Create();

        void SetLineWidth(float lineWidth);
        void SetCullMode(CullMode cullMode);
        void SetPrimitiveType(PrimitiveType primitiveType);
        void SetFrontFace(FrontFace frontFace);
        void SetSampleCount(SampleCount sampleCount);
        void SetDefaultAttribute();
        
        void EnableWireframe(bool wireframe);
        void EnableDepthTestEnable(bool depthTestEnable);
        void EnableDepthWriteEnable(bool depthWriteEnable);
        void EnableInstancing(bool enableInstancing);

        void AddLayout(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> attributes);

        const GraphicsPipeline& GetPipeline() const { return mPipeline; }

    private:
        float mLineWidth = 1.f;
        bool mDepthTestEnable = true;
        bool mDepthWriteEnable = true;
        bool mEnableInstancing = false;
        bool mWireframeEnable = false;

        CullMode mCullMode = CullMode::Back;
        PrimitiveType mPrimitiveType = PrimitiveType::Triangle;
        FrontFace mFrontFace = FrontFace::Clockwise;
        SampleCount mSampleCount = SampleCount::One; 

        GraphicsPipeline mPipeline;
        Descriptor mImageDescriptor;
        Texture mAlbedo;
        Sampler mAlbedoSampler;

        uint32_t mAttributeCount = 0;
};