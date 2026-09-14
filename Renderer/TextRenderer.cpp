#include "TextRenderer.hpp"
#include "Core/Application.hpp"
#include "Maths/Random.hpp"
#include "Renderer/Renderer.hpp"
#include <cstring>

struct TextVertex
{
    glm::vec3 position = glm::vec3(0);
    glm::vec2 uv = glm::vec2(0);

    static VertexLayout GetLayout(uint32_t binding, uint32_t startLocation)
    {
        VertexLayout layout;
        layout.attributes.emplace_back(binding, startLocation + 0, offsetof(TextVertex, position), ImageFormat::RGB32);
        layout.attributes.emplace_back(binding, startLocation + 1, offsetof(TextVertex, uv), ImageFormat::RG32);
        layout.bindings.emplace_back(binding, sizeof(TextVertex), InputRate::Vertex);

        return layout;
    }
};

void TextRenderer::Initialize()
{
    const Renderer &renderer = Application::GetInstance()->GetRenderer();

    std::vector<Vertex> vertices =
        {
            {{0.5, 0.5, 0.0}, {1, 0}, {}},
            {{0.5, -0.5, 0.0}, {1, 1}, {}},
            {{-0.5, -0.5, 0.0}, {0, 1}, {}},
            {{-0.5, 0.5, 0.0}, {0, 0}, {}},
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

    mUniformBuffer = UniformBuffer(sizeof(TextUniformData), &mUniformData);
    mUniformDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    mUniformDescriptor.Create();
    mUniformDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);

    mBezierDescriptor.AddDescriptor(DescriptorType::StorageBuffer, ShaderStage::Fragment);
    mBezierDescriptor.Create();

    ShaderConfig config =
        {
            .sampleCount = Application::GetInstance()->GetRenderer().GetSampleCount(),
            .cullMode = CullMode::None,
            .enableDepthWrite = true,
            .enableDepthTest = true,
            .descriptors = {mUniformDescriptor, mBezierDescriptor},
            .colorBlendAttachments = {true},
            .layouts = {TextVertex::GetLayout(0, 0), TextInstanceData::GetLayout(1, 2)},
            .pushConstantSize = sizeof(TextPushConstant),
        };

    mShader = Shader("Shaders/bezier.vert.spv", "Shaders/bezier.frag.spv", renderer.GetRenderPass(), renderer.GetRenderPassColorSubpassIndex(), config);
}

void TextRenderer::Terminate()
{
}

void TextRenderer::DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const glm::vec4 &forgroundColor, const glm::vec4 &backgroundColor, const Transform &transform)
{
    // const Font &font = FontManager::GetFont(id);

    mBezierDescriptor.UpdateBuffer(font.GetStorageBuffer().GetBuffer(), 0);

    Glyph glyph = font.GetGlyph(ch);

    glyph.size = glyph.size;
    glyph.bearing = glyph.bearing;
    glyph.advance = glyph.advance;

    float hSize = glyph.size.x * 0.5f;

    Transform glyphTransform;
    glyphTransform.position = position;
    glyphTransform.position.x += hSize + glyph.bearing.x;
    glyphTransform.position.y -= (glyph.size.y * 0.5f) - glyph.bearing.y;

    glyphTransform.scale = glm::vec3(glyph.size.x, glyph.size.y, 0);
    if (mPushConstant.mode == 1)
    {
    }

    TextInstanceData instanceData = {};
    instanceData.model = transform.GetMatrix() * glyphTransform.GetMatrix();
    instanceData.forgroundColor = forgroundColor;
    instanceData.backgroundColor = backgroundColor;
    if (ch != ' ')
    {
        instanceData.startIndex = font.GetGlyph(ch).contours[0].startIndex;
        for (int i = 0; i < font.GetGlyph(ch).contours.size(); i++)
        {
            instanceData.count += font.GetGlyph(ch).contours[i].count;
        }
    }
    mInstanceData.push_back(instanceData);
}

void TextRenderer::DrawText(const Font &font, const std::string &text, float spacing, const glm::vec4 &forgroundColor, const glm::vec4 &backgroundColor, const Transform &transform)
{
    // const Font &font = FontManager::GetFont(id);

    glm::vec3 position = glm::vec3(0);

    position.x = 0;

    for (char ch : text)
    {
        if (ch == '\n')
        {
            position.y -= font.GetMaxHeight();
            position.x = 0;

            continue;
        }
        const Glyph &glyph = font.GetGlyph(ch);
        DrawCharacter(font, ch, position, forgroundColor, backgroundColor, transform);
        position.x += font.GetGlyph(ch).advance.x * spacing;
    }
}

void TextRenderer::DrawText(const Font &font, const std::string &text, const TextProperty &property)
{
    // const Font &font = FontManager::GetFont(id);

    glm::vec3 position = glm::vec3(0);

    float totalSize = 0;
    for (char ch : text)
    {
        if (ch == '\n')
        {
            continue;
        }
        const Glyph &glyph = font.GetGlyph(ch);
        totalSize += font.GetGlyph(ch).advance.x * property.spacing;
    }

    totalSize = 0.f - (totalSize * 0.5f);
    position.x = totalSize;

    for (char ch : text)
    {
        if (ch == '\n')
        {
            position.y -= font.GetMaxHeight();
            position.x = 0;
            continue;
        }
        const Glyph &glyph = font.GetGlyph(ch);
        DrawCharacter(font, ch, position, property);
        position.x += font.GetGlyph(ch).advance.x * property.spacing;
    }
}

void TextRenderer::DrawCharacter(const Font &font, char ch, const glm::vec3 &position, const TextProperty &property)
{
    DrawCharacter(font, ch, position, property.forgroundColor, property.backgroundColor, property.transform);
}

void TextRenderer::DrawText(const Font &font, const std::string &text, const std::function<TextProperty(char ch, uint32_t index, const glm::vec2 &position, float totalSize)> &callback)
{
    // const Font &font = FontManager::GetFont(id);

    glm::vec3 position = glm::vec3(0);

    float totalSize = 0;
    for (char ch : text)
    {
        if (ch == '\n')
        {
            continue;
        }
        const Glyph &glyph = font.GetGlyph(ch);
        totalSize += font.GetGlyph(ch).advance.x * 1.f;
    }

    totalSize = 0.f - (totalSize * 0.5f);
    position.x = totalSize;

    for (int i = 0; i < text.size(); i++)
    {
        char ch = text[i];
        if (ch == '\n')
        {
            position.y -= font.GetMaxHeight();
            position.x = 0;
            continue;
        }

        TextProperty property = callback(ch, i, position, totalSize);
        const Glyph &glyph = font.GetGlyph(ch);
        DrawCharacter(font, ch, position, property);
        position.x += font.GetGlyph(ch).advance.x * property.spacing;
    }
}

void TextRenderer::Flush()
{
    if (mInstanceData.size() == 0)
    {
        return;
    }

    mInstanceBuffer.SetData(mInstanceData.data(), mInstanceData.size() * sizeof(TextInstanceData));

    RenderCommand renderCommand = {};
    renderCommand.descriptorCount = 2;
    renderCommand.descriptors[0] = &mUniformDescriptor;
    renderCommand.descriptors[1] = &mBezierDescriptor;
    renderCommand.indexBuffer = &mIndexBuffer;
    renderCommand.vertexBuffer = &mVertexBuffer;
    renderCommand.indexCount = mIndexBuffer.capacity / sizeof(uint32_t);
    renderCommand.instanceBuffer = &mInstanceBuffer;
    renderCommand.instanceCount = mInstanceData.size();
    renderCommand.pipeline = &mShader.GetGraphicsPipeline();
    renderCommand.pipelineSettings.cullMode = CullMode::None;
    renderCommand.pushContantSize = sizeof(TextPushConstant);
    renderCommand.pipelineSettings.enableDepthTest = false;
    renderCommand.pipelineSettings.enableDepthWrite = false;
    memcpy(renderCommand.pushContantData, &mPushConstant, sizeof(TextPushConstant));

    Application::GetInstance()->GetRenderer().Submit(renderCommand);
    mInstanceData.clear();
}

void TextRenderer::SetCamera(const Camera &camera)
{
    mUniformData.projection = camera.GetProjection();
    mUniformData.view = camera.GetView();

    mUniformBuffer.SetData(&mUniformData);
}

// UniformBuffer TextRenderer::mUniformBuffer;
// Descriptor TextRenderer::mUniformDescriptor;
// Descriptor TextRenderer::mBezierDescriptor;
// TextUniformData TextRenderer::mUniformData;
// Camera TextRenderer::mCamera;
// Shader TextRenderer::mShader;
// Buffer TextRenderer::mVertexBuffer;
// Buffer TextRenderer::mIndexBuffer;
// InstanceBuffer TextRenderer::mInstanceBuffer;
// std::vector<TextInstanceData> TextRenderer::mInstanceData;
// TextPushConstant TextRenderer::mPushConstant;