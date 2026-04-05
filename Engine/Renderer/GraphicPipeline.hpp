#pragma once 
#include "Renderer/PipelineLayout.hpp"
#include "Renderer/Shader.hpp"
#include "Renderer/VertexLayout.hpp"

enum class PrimitiveType
{
    Triangle,
    Line,
    Point
};

enum class CullMode
{
    None,
    Front,
    Back
};

enum class FrontFace
{
    Clockwise,
    CounterClockwise
};

class GraphicPipelineShader
{
    public:
        GraphicPipelineShader& SetVertexShader(const Shader& vertexShader) { mVertexShader = vertexShader; return *this; }
        GraphicPipelineShader& SetFragmentShader(const Shader& fragmentShader) { mFragmentShader = fragmentShader; return *this; }
        GraphicPipelineShader& SetPipelineLayout(const PipelineLayout& pipelineLayout) { mPipelineLayout = pipelineLayout; return *this; }

        const Shader& GetVertexShader() const { return mVertexShader; }
        const Shader& GetFragmentShader() const { return mFragmentShader; }
        const PipelineLayout& GetPipelineLayout() const { return mPipelineLayout; }

        void AddDescriptorDescription(DescriptorType type, uint32_t binding, ShaderType stage) 
        {
            mPipelineLayout.AddDescriptorInfo(type, binding, stage);
        }
        
    private:
        PipelineLayout mPipelineLayout;
        Shader mVertexShader;
        Shader mFragmentShader;
};

class GraphicPipeline
{
    public:
        GraphicPipeline& SetDepthEnabled(bool depthEnabled) { mDepthEnabled = depthEnabled; return *this; }
        GraphicPipeline& SetSampleCount(uint32_t sampleCount) { mSampleCount = sampleCount; return *this; }
        GraphicPipeline& SetId(uint64_t id) { mId = id; return *this; }
        GraphicPipeline& SetShader(const GraphicPipelineShader& shader) { mShader = shader; return *this; }
        GraphicPipeline& SetPrimitiveType(PrimitiveType primitiveType) { mPrimitiveType = primitiveType; return *this; }
        GraphicPipeline& SetCullMode(CullMode cullMode) { mCullMode = cullMode; return *this; }
        GraphicPipeline& SetFrontFace(FrontFace frontFace) { mFrontFace = frontFace; return *this; }
        GraphicPipeline& SetVertexLayout(const VertexLayout& vertexLayout) { mVertexLayout = vertexLayout; return *this; }

        bool IsDepthEnabled() const { return mDepthEnabled; }
        uint32_t GetSampleCount() const { return mSampleCount; }
        uint64_t GetId() const { return mId; }
        const GraphicPipelineShader& GetShader() const { return mShader; }
        PrimitiveType GetPrimitiveType() const { return mPrimitiveType; }
        CullMode GetCullMode() const { return mCullMode; }
        FrontFace GetFrontFace() const { return mFrontFace; }
        const VertexLayout& GetVertexLayout() const { return mVertexLayout; }

    private:
        GraphicPipelineShader mShader;
        VertexLayout mVertexLayout;

        PrimitiveType mPrimitiveType = PrimitiveType::Triangle;
        CullMode mCullMode = CullMode::None;
        FrontFace mFrontFace = FrontFace::CounterClockwise;


        bool mDepthEnabled = false;
        uint32_t mSampleCount = 1;
        uint64_t mId = 0;
};