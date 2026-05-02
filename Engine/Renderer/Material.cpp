#include "Material.hpp"
#include <Core/Application.hpp>

void Material::LoadAlbedo(std::string_view filename) 
{
    mAlbedo.Load(filename);    
}

void Material::LoadShaders(std::string_view vertexShaderFilename, std::string_view fragmentShaderFilename) 
{
    mPipeline.LoadVertexShader(vertexShaderFilename);
    mPipeline.LoadFragmentShader(fragmentShaderFilename);
}

void Material::Create() 
{
    mAlbedoSampler.Create();

    mImageDescriptor.AddDescriptor(DescriptorType::CombinedSampler, ShaderStage::Fragment);
    mImageDescriptor.Create();

    if(mAlbedo.IsValid())
        mImageDescriptor.UpdateImage(mAlbedo.GetImage(), ImageLayout::ShaderRead, mAlbedoSampler, 0);


    mPipeline.SetCullMode(mCullMode);
    mPipeline.SetPrimitive(mPrimitiveType);
    mPipeline.SetMultisampleCount(mSampleCount);
    mPipeline.EnableDepthWrite(mDepthWriteEnable);
    mPipeline.EnableDepthTesting(mDepthTestEnable);
    mPipeline.AddColorBlendAttachment(false);
    mPipeline.AddColorBlendAttachment(false);
    mPipeline.AddColorBlendAttachment(false);

    if(mAttributeCount == 0)
        SetDefaultAttribute();

    mPipeline.SetPipelineLayout(CreatePipelineLayout({mImageDescriptor.GetDescriptorSetLayout()}, {}));

    mPipeline.Create(Application::GetInstance()->GetRendererRef().GetDeferredRenderPass(), 0);
}

void Material::SetLineWidth(float lineWidth)
{
    mLineWidth = lineWidth; 
}
void Material::EnableDepthTestEnable(bool depthTestEnable)
{
    mDepthTestEnable = depthTestEnable; 
}
void Material::EnableDepthWriteEnable(bool depthWriteEnable)
{
    mDepthWriteEnable = depthWriteEnable; 
}
void Material::EnableInstancing(bool enableInstancing)
{
    mEnableInstancing = enableInstancing; 
}
void Material::EnableWireframe(bool wireframe)
{
    mWireframeEnable = wireframe; 
}
void Material::SetCullMode(CullMode cullMode)
{
    mCullMode = cullMode; 
}
void Material::SetPrimitiveType(PrimitiveType primitiveType)
{
    mPrimitiveType = primitiveType; 
}
void Material::SetFrontFace(FrontFace frontFace)
{
    mFrontFace = frontFace; 
}
void Material::SetSampleCount(SampleCount sampleCount)
{
    mSampleCount = sampleCount; 
}

ImageFormat GetAttributeFormat(AttributeType attributeType)
{
    ImageFormat formats[] = 
    {
        ImageFormat::R32U,      ImageFormat::R32U,      ImageFormat::R32,
        ImageFormat::RG32U,     ImageFormat::RG32U,     ImageFormat::RG32,
        ImageFormat::RGB32U,    ImageFormat::RGB32U,    ImageFormat::RGB32,
        ImageFormat::RGBA32U,   ImageFormat::RGBA32U,   ImageFormat::RGBA32,
    };

    return formats[(int)attributeType];
}

size_t GetAttributeSize(AttributeType attributeType)
{
    size_t sizes[] = 
    {
        sizeof(int) * 1, sizeof(uint32_t) * 1, sizeof(float) * 1,
        sizeof(int) * 2, sizeof(uint32_t) * 2, sizeof(float) * 2,
        sizeof(int) * 3, sizeof(uint32_t) * 3, sizeof(float) * 3,
        sizeof(int) * 4, sizeof(uint32_t) * 4, sizeof(float) * 4,
    };

    return sizes[(int)attributeType];
}

void Material::AddLayout(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> attributes) 
{
    size_t offset = 0;
    uint32_t location = 0;
    
    for (AttributeType attributeType : attributes)
    {
        mPipeline.AddAttribute(binding, location, GetAttributeFormat(attributeType), offset);
        location++;
        offset += GetAttributeSize(attributeType);
    }

    size_t stride = offset;
    mPipeline.AddBinding(binding, stride, inputRate);

    mAttributeCount++;
}

void Material::SetDefaultAttribute() 
{
    AddLayout(0, InputRate::Vertex, {AttributeType::Vec3, AttributeType::Vec2, AttributeType::Vec3});    
    if(mEnableInstancing)
        AddLayout(1, InputRate::Instance, {AttributeType::Vec4, AttributeType::Vec4, AttributeType::Vec4, AttributeType::Vec4});    
}
