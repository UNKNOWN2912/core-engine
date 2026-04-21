#include "Material.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/Mesh.hpp"
#include "Core/Application.hpp"


size_t attributeTypeSize[] = 
{
    4 * 1, 4 * 1, 4 * 1,
    4 * 2, 4 * 2, 4 * 2,
    4 * 3, 4 * 3, 4 * 3,
    4 * 4, 4 * 4, 4 * 4,
};

ImageFormat vulkanAttributeFormat[] = 
{
    ImageFormat::R32,    ImageFormat::R32U,    ImageFormat::R32,
    ImageFormat::RG32,   ImageFormat::RG32U,   ImageFormat::RG32,
    ImageFormat::RGB32,  ImageFormat::RGB32U,  ImageFormat::RGB32,
    ImageFormat::RGBA32, ImageFormat::RGBA32U, ImageFormat::RGBA32,
};


Material::Material()
{
    mPipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    mPipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mPipeline.AddAttribute(0, 1, ImageFormat::RG32,  offsetof(Vertex, uv));
    mPipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));

    if(mSettings.enableInstancing)
    {
        mPipeline.AddBinding(1, sizeof(glm::mat4), InputRate::Instance);
        mPipeline.AddAttribute(1, 3, ImageFormat::RGBA32, sizeof(glm::vec4) * 0);
        mPipeline.AddAttribute(1, 4, ImageFormat::RGBA32, sizeof(glm::vec4) * 1);
        mPipeline.AddAttribute(1, 5, ImageFormat::RGBA32, sizeof(glm::vec4) * 2);
        mPipeline.AddAttribute(1, 6, ImageFormat::RGBA32, sizeof(glm::vec4) * 3);
    }
}

void Material::LoadAlbedo(std::string_view filename)
{
    CHROME_TRACE_FUNCTION();
    mAlbedo.Load(filename);
}

void Material::LoadShaders(std::string_view vertexShader, std::string_view fragmentShader)
{
    CHROME_TRACE_FUNCTION();
    mPipeline.LoadVertexShader(vertexShader);
    mPipeline.LoadFragmentShader(fragmentShader);
}

void Material::ClearBindingAttribute() 
{
    mPipeline.ClearAttributesAndBinding();
}

void Material::SetBindingAttribute(uint32_t binding, InputRate inputRate, std::initializer_list<AttributeType> layout) 
{
    size_t stride = 0;

    for(AttributeType type : layout)
    {
        stride += attributeTypeSize[(uint32_t)type];
    }
    mPipeline.AddBinding(binding, stride, inputRate);

    int i = mFinalLocation;
    size_t offset = 0;
    for(AttributeType type : layout)
    {
        mPipeline.AddAttribute(binding, i, vulkanAttributeFormat[(uint32_t)type], offset);
        offset += attributeTypeSize[(uint32_t)type];
        i++;
    }
    mFinalLocation = i;
}


void Material::Create()
{
    CHROME_TRACE_FUNCTION();
 
    VkDescriptorPoolSize uniformPoolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 2,
    };
    VkDescriptorPoolSize samplerPoolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 2,
    };

    mDescriptorPool = CreateDescriptorPool({uniformPoolSize, samplerPoolSize}, 2);

    VkDescriptorSetLayoutBinding uniformBinding = 
    {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutBinding albedoBinding = 
    {
        .binding = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };

    VkDescriptorSetLayoutBinding shadowUniformBinding = 
    {
        .binding = 2,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorSetLayoutBinding shadowMapBinding = 
    {
        .binding = 3,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
    };


    mSetLayout = CreateDescriptorSetLayout({uniformBinding, albedoBinding, shadowUniformBinding, shadowMapBinding});

    mDescriptorSet = AllocateDescriptorSet(mSetLayout, mDescriptorPool);


    VkPushConstantRange pushConstantRange =
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(glm::mat4),
    };

    mPipelineLayout = CreatePipelineLayout({mSetLayout}, {pushConstantRange});
    
    mPipeline.EnableDepthTesting(mSettings.depthTestEnable);
    mPipeline.EnableDepthWrite(mSettings.depthWriteEnable);
    mPipeline.EnableBlending(mSettings.blendEnable);
    mPipeline.EnableWireframe(mSettings.wireframe);

    mPipeline.AddColorBlendAttachment(mSettings.blendEnable);

    mPipeline.SetCullMode(mSettings.cullMode);


    mPipeline.SetPrimitive(mSettings.primitiveType);
    mPipeline.SetMultisampleCount(mSettings.sampleCount);

    mPipeline.SetPipelineLayout(mPipelineLayout);

    Camera camera = Application::GetInstance()->GetRendererRef().GetCamera();

    RenderPass renderPass = Application::GetInstance()->GetRendererRef().GetMainRenderPass();

    mPipeline.Create(renderPass, 0);

    if(mAlbedo.IsValid())
        mAlbedo.UpdateDescriptorSet(mDescriptorSet, 1);

    mIsValid = true;
}

MaterialSettings& Material::GetSettingsRef()
{
    CHROME_TRACE_FUNCTION();
    return mSettings;
}

void Material::SetAlbedoSampler(Filter mag, Filter min, std::array<AddressMode, 3> addressModes) 
{
    mAlbedo.SetSampler(mag, min, {addressModes[0], addressModes[1], addressModes[2]});
}
