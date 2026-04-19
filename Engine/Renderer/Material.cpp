#include "Material.hpp"
#include "Renderer/Mesh.hpp"
#include "Core/Application.hpp"


size_t attributeTypeSize[] = 
{
    4 * 1, 4 * 1, 4 * 1,
    4 * 2, 4 * 2, 4 * 2,
    4 * 3, 4 * 3, 4 * 3,
    4 * 4, 4 * 4, 4 * 4,
};

VkVertexInputRate vulkanVertexInputRate[]  = 
{
    VK_VERTEX_INPUT_RATE_MAX_ENUM,
    VK_VERTEX_INPUT_RATE_VERTEX,
    VK_VERTEX_INPUT_RATE_INSTANCE
};

VkFormat vulkanAttributeFormat[] = 
{
    VK_FORMAT_R32_SINT,             VK_FORMAT_R32_UINT,             VK_FORMAT_R32_SFLOAT,
    VK_FORMAT_R32G32_SINT,          VK_FORMAT_R32G32_UINT,          VK_FORMAT_R32G32_SFLOAT,
    VK_FORMAT_R32G32B32_SINT,       VK_FORMAT_R32G32B32_UINT,       VK_FORMAT_R32G32B32_SFLOAT,
    VK_FORMAT_R32G32B32A32_SINT,    VK_FORMAT_R32G32B32A32_UINT,    VK_FORMAT_R32G32B32A32_SFLOAT,
};


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
    ERROR("Invalid cull mode");
    return  VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
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

    ERROR("Invalid primitive");
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
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

    ERROR("Invalid front face");
    return VK_FRONT_FACE_MAX_ENUM;
}

Material::Material()
{
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
}

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

    mPipeline.AddBinding(binding, stride, vulkanVertexInputRate[(uint32_t)inputRate]);

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


    mIsValid = true;
}

MaterialSettings& Material::GetSettingsRef()
{
    CHROME_TRACE_FUNCTION();
    return mSettings;
}

VkFilter vulkanFilter[]
{
    VK_FILTER_MAX_ENUM,
    VK_FILTER_NEAREST,
    VK_FILTER_LINEAR
};

VkSamplerAddressMode vulkanAddressMode[]
{
    VK_SAMPLER_ADDRESS_MODE_MAX_ENUM,
    VK_SAMPLER_ADDRESS_MODE_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
};

void Material::SetAlbedoSampler(Filter mag, Filter min, std::array<AddressMode, 3> addressModes) 
{
    mAlbedo.SetSampler(vulkanFilter[(uint32_t)min], vulkanFilter[(uint32_t)mag], {vulkanAddressMode[(uint32_t)addressModes[0]], vulkanAddressMode[(uint32_t)addressModes[0]], vulkanAddressMode[(uint32_t)addressModes[0]]});
}
