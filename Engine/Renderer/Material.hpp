#pragma once
#include "Renderer/Descriptor.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/InstanceBuffer.hpp"
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
        void Destroy();

        void SetLineWidth(float lineWidth);
        void SetCullMode(CullMode cullMode);
        void SetPrimitiveType(PrimitiveType primitiveType);
        void SetFrontFace(FrontFace frontFace);
        void SetSampleCount(SampleCount sampleCount);
        void SetDefaultAttribute();

        void SetInstanceCount(uint32_t instanceCount) { mInstanceCount = instanceCount; }
        
        void EnableWireframe(bool wireframe);
        void EnableDepthTestEnable(bool depthTestEnable);
        void EnableDepthWriteEnable(bool depthWriteEnable);
        void EnableInstancing(bool enableInstancing);

        void AddLayout(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> attributes);

        const GraphicsPipeline& GetPipeline() const { return mPipeline; }
        const Descriptor& GetImageDescriptor() const { return mImageDescriptor; }
        const Descriptor& GetUniformDescriptor() const { return mUniformDescriptor; }

        uint32_t GetInstanceCount() const { return mInstanceCount; }

        void SetInstanceData(void* data, size_t size);
        void SetInstanceBuffer(const InstanceBuffer& instanceBuffer);

        bool IsInstancingEnabled() const { return mEnableInstancing; }

        const InstanceBuffer& GetInstanceBuffer() const { return mInstanceBuffer; }

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
        Descriptor mUniformDescriptor;

        Texture mAlbedo;
        Sampler mAlbedoSampler;

        InstanceBuffer mInstanceBuffer;
        uint32_t mInstanceCount = 0;

        uint32_t mAttributeCount = 0;

        uint32_t mLastAttributeLocation = 0;
};