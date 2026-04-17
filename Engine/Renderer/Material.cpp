#include "Material.hpp"
#include "Renderer/Mesh.hpp"
#include "Core/Application.hpp"


void Material::LoadAlbedo(std::string_view filename)
{
    CHROME_TRACE_FUNCTION();
    mAlbedo.Load(filename, 0);
}

void Material::LoadShaders(std::string_view vertexShader, std::string_view fragmentShader)
{
    CHROME_TRACE_FUNCTION();
    mPipeline.LoadVertexShader(vertexShader);
    mPipeline.LoadFragmentShader(fragmentShader);
}

VkCullModeFlags GetCullMode(CullMode mode)
{
    CHROME_TRACE_FUNCTION();
    switch (mode) 
    {
        case CullMode::None:
            return VK_CULL_MODE_NONE;
            break;
        case CullMode::Front:
            return VK_CULL_MODE_FRONT_BIT;
            break;
        case CullMode::Back:
            return VK_CULL_MODE_BACK_BIT;
            break;
    }
}

VkPrimitiveTopology GetPrimitive(PrimitiveType primitive)
{
    CHROME_TRACE_FUNCTION();
    switch (primitive)
    {
        case PrimitiveType::None:
            return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
            break;
        case PrimitiveType::Triangle:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case PrimitiveType::Line:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case PrimitiveType::Point:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
    };
}

VkFrontFace GetFrontFace(FrontFace face)
{
    CHROME_TRACE_FUNCTION();
    switch (face) 
    {
        case FrontFace::None:
            return VK_FRONT_FACE_MAX_ENUM;
            break;
        case FrontFace::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
            break;
        case FrontFace::CounterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
            break;
    }
}

void Material::Create()
{
    CHROME_TRACE_FUNCTION();
 
    VkDescriptorPoolSize uniformPoolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };
    VkDescriptorPoolSize samplerPoolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = 1,
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


    mSetLayout = CreateDescriptorSetLayout({uniformBinding, albedoBinding});

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

    mPipeline.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    mPipeline.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
    mPipeline.AddAttribute(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));
    mPipeline.AddAttribute(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));

    if(mSettings.enableInstancing)
    {
        mPipeline.AddBinding(1, sizeof(glm::mat4), VK_VERTEX_INPUT_RATE_INSTANCE);
        mPipeline.AddAttribute(1, 3, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) * 0);
        mPipeline.AddAttribute(1, 4, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) * 1);
        mPipeline.AddAttribute(1, 5, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) * 2);
        mPipeline.AddAttribute(1, 6, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(glm::vec4) * 3);
    }

    mPipeline.AddColorBlendAttachment(mSettings.blendEnable);

    mPipeline.SetCullMode(GetCullMode(mSettings.cullMode));


    mPipeline.SetPrimitive(GetPrimitive(mSettings.primitiveType));
    mPipeline.SetMultisampleCount(mSettings.sampleCount);

    mPipeline.SetPipelineLayout(mPipelineLayout);

    Camera camera = Application::GetInstance()->GetRendererRef().GetCamera();

    VkRenderPass renderPass = Application::GetInstance()->GetRendererRef().GetMainRenderPass();

    mPipeline.Create(renderPass, 0);

    if(mAlbedo.IsValid())
        mAlbedo.SetDataToDescriptorSet(mDescriptorSet, 1);
}

MaterialSettings& Material::GetSettingsRef()
{
    CHROME_TRACE_FUNCTION();
    return mSettings;
}
