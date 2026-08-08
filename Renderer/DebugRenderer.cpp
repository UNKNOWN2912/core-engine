#include "DebugRenderer.hpp"
#include "Maths/Constants.hpp"
#include "Renderer.hpp"
#include <Assets/ShaderManager.hpp>

void DebugRenderer::Initialize()
{
    mDebugLineId = ShaderManager::Load("debugLine", "Shaders/debugLine.vert.spv", "Shaders/debugLine.frag.spv", false);

    mLinePipeline.SetVertexShader(ShaderManager::Get(mDebugLineId).vertex);
    mLinePipeline.SetFragmentShader(ShaderManager::Get(mDebugLineId).fragment);
    mLinePipeline.AddBinding(0, sizeof(LineVertex), InputRate::Vertex);
    mLinePipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(LineVertex, position));
    mLinePipeline.AddAttribute(0, 1, ImageFormat::RGB32, offsetof(LineVertex, color));
    mLinePipeline.AddColorBlendAttachment(false);
    mLinePipeline.SetPrimitive(PrimitiveType::Line);
    mLinePipeline.AddDescriptors(Renderer::GetBufferDescriptor());
    mLinePipeline.SetMultisampleCount(Renderer::GetSampleCount());
    mLinePipeline.CreatePipeline(Renderer::GetRenderPass(), 0);
}

void DebugRenderer::Terminate()
{
}

void DebugRenderer::Enable(bool enable)
{
    mEnabled = enable;
}

void DebugRenderer::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    if (!mEnabled)
        return;

    mLineVertices.push_back({start, color});
    mLineVertices.push_back({end, color});

    mLineIndices.push_back(mLineIndices.size());
    mLineIndices.push_back(mLineIndices.size());
}
void DebugRenderer::DrawWireframe(std::string_view mesh)
{
    if (!mEnabled)
        return;
}
void DebugRenderer::DrawPoint(const glm::vec3 &position, const glm::vec3 &color)
{
    if (!mEnabled)
        return;
}
void DebugRenderer::DrawCuboid(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    if (!mEnabled)
        return;

    DrawLine({start.x, start.y, start.z}, {end.x, start.y, start.z}, color);
    DrawLine({start.x, start.y, start.z}, {start.x, end.y, start.z}, color);

    DrawLine({end.x, end.y, start.z}, {end.x, start.y, start.z}, color);
    DrawLine({end.x, end.y, start.z}, {start.x, end.y, start.z}, color);

    DrawLine({end.x, end.y, end.z}, {end.x, start.y, end.z}, color);
    DrawLine({end.x, end.y, end.z}, {start.x, end.y, end.z}, color);

    DrawLine({start.x, start.y, end.z}, {end.x, start.y, end.z}, color);
    DrawLine({start.x, start.y, end.z}, {start.x, end.y, end.z}, color);

    DrawLine({start.x, start.y, start.z}, {start.x, start.y, end.z}, color);
    DrawLine({end.x, start.y, start.z}, {end.x, start.y, end.z}, color);
    DrawLine({end.x, end.y, start.z}, {end.x, end.y, end.z}, color);
    DrawLine({start.x, end.y, start.z}, {start.x, end.y, end.z}, color);
}
void DebugRenderer::DrawCircleXY(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount)
{
    if (!mEnabled)
        return;

    float pi2 = 2 * pi;

    auto getCirclePoint = [&](float a, float radius) {
        return glm::vec3(glm::sin(a * pi2), glm::cos(a * pi2), 0) * radius;
    };

    for (int i = 1; i < lineCount + 1; i++)
    {
        float startA = float(i - 1) / float(lineCount);
        float endA = float(i) / float(lineCount);

        glm::vec3 start = getCirclePoint(startA, radius) + position;
        glm::vec3 end = getCirclePoint(endA, radius) + position;

        DrawLine(start, end, color);
    }
}
void DebugRenderer::DrawCircleZY(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount)
{
    if (!mEnabled)
        return;

    float pi2 = 2 * pi;

    auto getCirclePoint = [&](float a, float radius) {
        return glm::vec3(0, glm::sin(a * pi2), glm::cos(a * pi2)) * radius;
    };

    for (int i = 1; i < lineCount + 1; i++)
    {
        float startA = float(i - 1) / float(lineCount);
        float endA = float(i) / float(lineCount);

        glm::vec3 start = getCirclePoint(startA, radius) + position;
        glm::vec3 end = getCirclePoint(endA, radius) + position;

        DrawLine(start, end, color);
    }
}
void DebugRenderer::DrawCircleXZ(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount)
{
    if (!mEnabled)
        return;

    float pi2 = 2 * pi;

    auto getCirclePoint = [&](float a, float radius) {
        return glm::vec3(glm::sin(a * pi2), 0, glm::cos(a * pi2)) * radius;
    };

    for (int i = 1; i < lineCount + 1; i++)
    {
        float startA = float(i - 1) / float(lineCount);
        float endA = float(i) / float(lineCount);

        glm::vec3 start = getCirclePoint(startA, radius) + position;
        glm::vec3 end = getCirclePoint(endA, radius) + position;

        DrawLine(start, end, color);
    }
}

void DebugRenderer::DrawCube(const glm::vec3 &position, float size, const glm::vec3 &color)
{
    if (!mEnabled)
        return;

    glm::vec3 start = position + glm::vec3(size * 0.5);
    glm::vec3 end = position - glm::vec3(size * 0.5);

    DrawCuboid(start, end, color);
}

void DebugRenderer::DrawFrustrum(const glm::vec3 &start1, const glm::vec3 &end1, const glm::vec3 &start2, const glm::vec3 &end2, const glm::vec3 &color)
{
    if (!mEnabled)
        return;

    DrawLine({start1.x, start1.y, start1.z}, {end1.x, start1.y, start1.z}, color);
    DrawLine({start1.x, start1.y, start1.z}, {start1.x, end1.y, start1.z}, color);

    DrawLine({end1.x, end1.y, end1.z}, {end1.x, start1.y, end1.z}, color);
    DrawLine({end1.x, end1.y, end1.z}, {start1.x, end1.y, end1.z}, color);

    DrawLine({start2.x, start2.y, start2.z}, {end2.x, start2.y, start2.z}, color);
    DrawLine({start2.x, start2.y, start2.z}, {start2.x, end2.y, start2.z}, color);

    DrawLine({end2.x, end2.y, end2.z}, {end2.x, start2.y, end2.z}, color);
    DrawLine({end2.x, end2.y, end2.z}, {start2.x, end2.y, end2.z}, color);

    DrawLine({start1.x, start1.y, start1.z}, {start2.x, start2.y, start2.z}, color);
    DrawLine({end1.x, end1.y, end1.z}, {end2.x, end2.y, end2.z}, color);
    DrawLine({end1.x, start1.y, start1.z}, {end2.x, start2.y, start2.z}, color);
    DrawLine({start1.x, end1.y, start1.z}, {start2.x, end2.y, start2.z}, color);
}

void DebugRenderer::Flush()
{
    if (mLineVertices.size() != 0)
    {
        mLineMesh.SetData(mLineVertices.data(), sizeof(LineVertex) * mLineVertices.size(), mLineIndices.data(), mLineIndices.size() * sizeof(uint32_t));

        RenderCommand renderCommand;
        renderCommand.debugName = "DebugLineRenderer";
        renderCommand.vertexBuffer = &mLineMesh.GetVertexBuffer();
        renderCommand.indexBuffer = &mLineMesh.GetIndexBuffer();
        renderCommand.indexCount = mLineIndices.size();
        renderCommand.pipeline = &mLinePipeline;
        renderCommand.descriptors[0] = &Renderer::GetBufferDescriptor();
        renderCommand.descriptorCount = 1;

        renderCommand.pipelineSettings.cullMode = CullMode::None;

        Renderer::Submit(renderCommand);

        mLineVertices.clear();
        mLineIndices.clear();
    }
}
