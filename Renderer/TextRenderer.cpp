#include "TextRenderer.hpp"
#include "Renderer/Renderer.hpp"
#include <cstring>

struct TextVertex
{
    glm::vec3 position;
    glm::vec2 uv;
};

void TextRenderer::Initialize()
{
    std::vector<Vertex> vertices =
        {
            {{0.5, 0.5, 0.0}, {1, 0}, {}, {}, {}},
            {{0.5, -0.5, 0.0}, {1, 1}, {}, {}, {}},
            {{-0.5, -0.5, 0.0}, {0, 1}, {}, {}, {}},
            {{-0.5, 0.5, 0.0}, {0, 0}, {}, {}, {}},
        };

    std::vector<TextVertex> textVertices =
        {
            {{0.5, 0.5, 0.0}, {1, 0}},
            {{0.5, -0.5, 0.0}, {1, 1}},
            {{-0.5, -0.5, 0.0}, {0, 1}},
            {{-0.5, 0.5, 0.0}, {0, 0}},
        };

    std::vector<uint32_t> indices =
        {
            0,
            1,
            2,
            2,
            3,
            0,
        };

    Buffer mStagingVertexBuffer = CreateBuffer(sizeof(TextVertex) * textVertices.size(), BufferUsage::TransferSource, MemoryProperty::HostVisible | MemoryProperty::HostCoherent);
    memcpy(mStagingVertexBuffer.map, textVertices.data(), sizeof(TextVertex) * textVertices.size());

    Buffer mStagingIndexBuffer = CreateBuffer(sizeof(uint32_t) * indices.size(), BufferUsage::TransferSource, MemoryProperty::HostVisible | MemoryProperty::HostCoherent);
    memcpy(mStagingIndexBuffer.map, indices.data(), sizeof(uint32_t) * indices.size());

    mVertexBuffer = CreateBuffer(sizeof(TextVertex) * textVertices.size(), BufferUsage::VertexBuffer | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);
    mIndexBuffer = CreateBuffer(sizeof(uint32_t) * indices.size(), BufferUsage::IndexBuffer | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);

    TransferBufferData(mStagingVertexBuffer, mVertexBuffer);
    TransferBufferData(mStagingIndexBuffer, mIndexBuffer);

    DestroyBuffer(mStagingVertexBuffer);
    DestroyBuffer(mStagingIndexBuffer);

    mQuadMeshId = MeshManager::CreateMesh(vertices, indices);

    mShaderID = ShaderManager::Load("Shaders/text.vert.spv", "Shaders/text.frag.spv", false);

    mUniformBuffer = UniformBuffer(sizeof(TextUniformData), &mUniformData);

    mUniformDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    mUniformDescriptor.CreateDescriptor();
    mUniformDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);

    mTextPipeline.AddBinding(0, sizeof(TextVertex), InputRate::Vertex);
    mTextPipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mTextPipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    mTextPipeline.AddBinding(1, sizeof(TextInstanceData), InputRate::Instance);
    mTextPipeline.AddAttribute(1, 2, ImageFormat::R32U, offsetof(TextInstanceData, charaterTextureId));
    mTextPipeline.AddAttribute(1, 3, ImageFormat::RGBA32, offsetof(TextInstanceData, model) + (sizeof(glm::vec4) * 0));
    mTextPipeline.AddAttribute(1, 4, ImageFormat::RGBA32, offsetof(TextInstanceData, model) + (sizeof(glm::vec4) * 1));
    mTextPipeline.AddAttribute(1, 5, ImageFormat::RGBA32, offsetof(TextInstanceData, model) + (sizeof(glm::vec4) * 2));
    mTextPipeline.AddAttribute(1, 6, ImageFormat::RGBA32, offsetof(TextInstanceData, model) + (sizeof(glm::vec4) * 3));
    mTextPipeline.AddAttribute(1, 7, ImageFormat::RGBA32, offsetof(TextInstanceData, color));
    mTextPipeline.AddDescriptors(TextureManager::GetDescriptor());
    mTextPipeline.AddDescriptors(mUniformDescriptor);

    mTextPipeline.AddColorBlendAttachment(true);

    mTextPipeline.EnableDepthTesting(true);
    mTextPipeline.EnableDepthWrite(true);
    mTextPipeline.SetCullMode(CullMode::None);

    mTextPipeline.SetVertexShader(ShaderManager::Get(mShaderID).vertex);
    mTextPipeline.SetFragmentShader(ShaderManager::Get(mShaderID).fragment);

    mTextPipeline.SetMultisampleCount(Renderer::GetSampleCount());

    mTextPipeline.CreatePipeline(Renderer::GetRenderPass(), 0);
}

void TextRenderer::Terminate()
{
}

void TextRenderer::DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const glm::vec4 &color, const Transform &transform)
{
    Glyph glyph = font.GetGlyph(ch);

    float size = glyph.pixelSize;
    glyph.size = glyph.size / size;
    glyph.bearing = glyph.bearing / size;
    glyph.advance = glyph.advance / size;
    glyph.textureId = glyph.textureId;

    float aspectRatio = glyph.size.x / glyph.size.y;

    float hSize = glyph.size.x * 0.5f;

    Transform glyphTransform;
    glyphTransform.position = position;
    glyphTransform.position.x += hSize + glyph.bearing.x;
    glyphTransform.position.y -= (glyph.size.y * 0.5f) - glyph.bearing.y;
    glyphTransform.scale = glm::vec3(glyph.size.x, glyph.size.y, 0);

    TextInstanceData instanceData = {};
    instanceData.charaterTextureId = (uint32_t)font.GetGlyph(ch).textureId;
    instanceData.model = transform.GetMatrix() * glyphTransform.GetMatrix();
    instanceData.color = color;
    mInstanceData.push_back(instanceData);
}

void TextRenderer::DrawText(const Font &font, const std::string &text, float spacing, const glm::vec4 &color, const Transform &transform)
{
    glm::vec3 position = glm::vec3(0);

    float totalSize = 0;
    for (char ch : text)
    {
        const Glyph &glyph = font.GetGlyph(ch);
        totalSize += (font.GetGlyph(ch).advance.x / glyph.pixelSize) * spacing;
    }

    totalSize = 0.f - (totalSize * 0.5f);
    position.x = totalSize;

    for (char ch : text)
    {
        const Glyph &glyph = font.GetGlyph(ch);
        float size = glyph.pixelSize;
        DrawCharacter(font, ch, position, color, transform);
        position.x += (font.GetGlyph(ch).advance.x / size) * spacing;
    }
}

void TextRenderer::Flush()
{
    if (mInstanceData.size() == 0)
    {
        return;
    }

    const std::shared_ptr<Mesh> quad = MeshManager::GetMesh(mQuadMeshId);

    mInstanceBuffer.SetData(mInstanceData.data(), mInstanceData.size() * sizeof(TextInstanceData));

    RenderCommand renderCommand = {};
    renderCommand.descriptorCount = 2;
    renderCommand.descriptors[0] = &TextureManager::GetDescriptor();
    renderCommand.descriptors[1] = &mUniformDescriptor;
    renderCommand.indexBuffer = &mIndexBuffer;
    renderCommand.vertexBuffer = &mVertexBuffer;
    renderCommand.indexCount = quad->GetIndexBuffer().size / sizeof(uint32_t);
    renderCommand.instanceBuffer = &mInstanceBuffer;
    renderCommand.instanceCount = mInstanceData.size();
    renderCommand.pipeline = &mTextPipeline;
    renderCommand.pipelineSettings.cullMode = CullMode::None;

    Renderer::Submit(renderCommand);
    mInstanceData.clear();
}

void TextRenderer::SetCamera(const Camera &camera)
{
    mUniformData.projection = camera.GetProjection();
    mUniformData.view = camera.GetView();

    mUniformBuffer.SetData(&mUniformData);
}

GraphicsPipeline TextRenderer::mTextPipeline;
UniformBuffer TextRenderer::mUniformBuffer;
Descriptor TextRenderer::mUniformDescriptor;
TextUniformData TextRenderer::mUniformData;
Camera TextRenderer::mCamera;
ShaderID TextRenderer::mShaderID;
Buffer TextRenderer::mVertexBuffer;
Buffer TextRenderer::mIndexBuffer;
MeshID TextRenderer::mQuadMeshId;
InstanceBuffer TextRenderer::mInstanceBuffer;
std::vector<TextInstanceData> TextRenderer::mInstanceData;