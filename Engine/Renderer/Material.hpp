#pragma once
#include "Assets/ShaderManager.hpp"
#include "Assets/TextureManager.hpp"
#include "Renderer/Descriptor.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Sampler.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Types.hpp"
#include <memory>

enum class AttributeType
{
    Int,
    UInt,
    Float,
    IVec2,
    UVec2,
    Vec2,
    IVec3,
    UVec3,
    Vec3,
    IVec4,
    UVec4,
    Vec4,
};

class Material
{
  public:
    void SetAlbedo(TextureID id);
    void SetShaders(VertexShaderID vertexShaderId, FragmentShaderID fragmentShaderId);

    void Create();
    void Destroy();

    void SetLineWidth(float lineWidth);
    void SetCullMode(CullMode cullMode);
    void SetPrimitiveType(PrimitiveType primitiveType);
    void SetFrontFace(FrontFace frontFace);
    void SetSampleCount(SampleCount sampleCount);
    void SetDefaultAttribute();

    void SetInstanceCount(uint32_t instanceCount)
    {
        mInstanceCount = instanceCount;
    }

    void EnableWireframe(bool wireframe);
    void EnableDepthTestEnable(bool depthTestEnable);
    void EnableDepthWriteEnable(bool depthWriteEnable);
    void EnableInstancing(bool enableInstancing);

    void AddLayout(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> attributes);

    const GraphicsPipeline &GetPipeline() const
    {
        return mPipeline;
    }
    const Descriptor &GetImageDescriptor() const
    {
        return mImageDescriptor;
    }
    const Descriptor &GetUniformDescriptor() const
    {
        return mUniformDescriptor;
    }

    GraphicsPipeline &GetPipelineRef()
    {
        return mPipeline;
    }
    Descriptor &GetImageDescriptorRef()
    {
        return mImageDescriptor;
    }
    Descriptor &GetUniformDescriptorRef()
    {
        return mUniformDescriptor;
    }

    uint32_t GetInstanceCount() const
    {
        return mInstanceCount;
    }

    void SetInstanceData(void *data, size_t size);
    void SetInstanceBuffer(const InstanceBuffer &instanceBuffer);

    bool IsInstancingEnabled() const
    {
        return mEnableInstancing;
    }

    const InstanceBuffer &GetInstanceBuffer() const
    {
        return mInstanceBuffer;
    }
    InstanceBuffer &GetInstanceBufferRef()
    {
        return mInstanceBuffer;
    }

    const std::string &GetName() const
    {
        return mName;
    }

    void SetName(std::string_view name)
    {
        mName = name;
    }

  private:
    std::string mName;
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

    Sampler mAlbedoSampler;

    TextureID mAlbedo = TextureManager::GetInvalidID();

    InstanceBuffer mInstanceBuffer;
    uint32_t mInstanceCount = 0;

    uint32_t mAttributeCount = 0;

    uint32_t mLastAttributeLocation = 0;
};