#include "DebugRenderer.hpp"
#include "Core/Application.hpp"
#include "Maths/Constants.hpp"
#include "Renderer.hpp"
#include <Assets/ShaderManager.hpp>

void DebugRenderer::Initialize()
{
    Renderer &renderer = Application::GetInstance()->GetRenderer();

    mLineShader = Shader("Shaders/debugLine.vert.spv", "Shaders/debugLine.frag.spv",
                         renderer.GetRenderPass(), Application::GetInstance()->GetRenderer().GetRenderPassColorSubpassIndex(),
                         ShaderConfig{
                             .sampleCount = renderer.GetSampleCount(),
                             .primitive = PrimitiveType::Line,
                             .descriptors = {renderer.GetBufferDescriptor()},
                             .colorBlendAttachments = {false},
                             .layouts = {LineVertex::GetLayout(0, 0)},
                         });
}

void DebugRenderer::Terminate()
{
}

void DebugRenderer::Enable(bool enable)
{
    mEnabled = enable;
}

void DebugRenderer::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color, bool overlay)
{
    if (!mEnabled)
        return;

    mLineVertices.push_back({start, color, overlay});
    mLineVertices.push_back({end, color, overlay});

    mLineIndices.push_back(mLineIndices.size());
    mLineIndices.push_back(mLineIndices.size());
}
void DebugRenderer::DrawWireframe(std::string_view mesh, bool overlay)
{
    if (!mEnabled)
        return;
}
void DebugRenderer::DrawPoint(const glm::vec3 &position, const glm::vec3 &color, bool overlay)
{
    if (!mEnabled)
        return;
}
void DebugRenderer::DrawCuboid(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color, bool overlay)
{
    if (!mEnabled)
        return;

    DrawLine({start.x, start.y, start.z}, {end.x, start.y, start.z}, color, overlay);
    DrawLine({start.x, start.y, start.z}, {start.x, end.y, start.z}, color, overlay);

    DrawLine({end.x, end.y, start.z}, {end.x, start.y, start.z}, color, overlay);
    DrawLine({end.x, end.y, start.z}, {start.x, end.y, start.z}, color, overlay);

    DrawLine({end.x, end.y, end.z}, {end.x, start.y, end.z}, color, overlay);
    DrawLine({end.x, end.y, end.z}, {start.x, end.y, end.z}, color, overlay);

    DrawLine({start.x, start.y, end.z}, {end.x, start.y, end.z}, color, overlay);
    DrawLine({start.x, start.y, end.z}, {start.x, end.y, end.z}, color, overlay);

    DrawLine({start.x, start.y, start.z}, {start.x, start.y, end.z}, color, overlay);
    DrawLine({end.x, start.y, start.z}, {end.x, start.y, end.z}, color, overlay);

    DrawLine({end.x, end.y, start.z}, {end.x, end.y, end.z}, color, overlay);
    DrawLine({start.x, end.y, start.z}, {start.x, end.y, end.z}, color, overlay);
}
void DebugRenderer::DrawCircleXY(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay, uint32_t lineCount)
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

        DrawLine(start, end, color, overlay);
    }
}
void DebugRenderer::DrawCircleZY(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay, uint32_t lineCount)
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

        DrawLine(start, end, color, overlay);
    }
}
void DebugRenderer::DrawCircleXZ(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay, uint32_t lineCount)
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

        DrawLine(start, end, color, overlay);
    }
}

void DebugRenderer::DrawCube(const glm::vec3 &position, float size, const glm::vec3 &color, bool overlay)
{
    if (!mEnabled)
        return;

    glm::vec3 start = position + glm::vec3(size * 0.5);
    glm::vec3 end = position - glm::vec3(size * 0.5);

    DrawCuboid(start, end, color, overlay);
}

void DebugRenderer::DrawFrustrum(const glm::vec3 &start1, const glm::vec3 &end1, const glm::vec3 &start2, const glm::vec3 &end2, const glm::vec3 &color, bool overlay)
{
    if (!mEnabled)
        return;

    DrawLine({start1.x, start1.y, start1.z}, {end1.x, start1.y, start1.z}, color, overlay);
    DrawLine({start1.x, start1.y, start1.z}, {start1.x, end1.y, start1.z}, color, overlay);

    DrawLine({end1.x, end1.y, end1.z}, {end1.x, start1.y, end1.z}, color, overlay);
    DrawLine({end1.x, end1.y, end1.z}, {start1.x, end1.y, end1.z}, color, overlay);

    DrawLine({start2.x, start2.y, start2.z}, {end2.x, start2.y, start2.z}, color, overlay);
    DrawLine({start2.x, start2.y, start2.z}, {start2.x, end2.y, start2.z}, color, overlay);

    DrawLine({end2.x, end2.y, end2.z}, {end2.x, start2.y, end2.z}, color, overlay);
    DrawLine({end2.x, end2.y, end2.z}, {start2.x, end2.y, end2.z}, color, overlay);

    DrawLine({start1.x, start1.y, start1.z}, {start2.x, start2.y, start2.z}, color, overlay);
    DrawLine({end1.x, end1.y, end1.z}, {end2.x, end2.y, end2.z}, color, overlay);
    DrawLine({end1.x, start1.y, start1.z}, {end2.x, start2.y, start2.z}, color, overlay);
    DrawLine({start1.x, end1.y, start1.z}, {start2.x, end2.y, start2.z}, color, overlay);
}

void DebugRenderer::Flush()
{
    Renderer &renderer = Application::GetInstance()->GetRenderer();

    if (mLineVertices.size() != 0)
    {
        mLineMesh.SetData(mLineVertices.data(), sizeof(LineVertex) * mLineVertices.size(), mLineIndices.data(), mLineIndices.size() * sizeof(uint32_t));

        RenderCommand renderCommand;
        renderCommand.debugName = "DebugLineRenderer";
        renderCommand.vertexBuffer = &mLineMesh.GetVertexBuffer();
        renderCommand.indexBuffer = &mLineMesh.GetIndexBuffer();
        renderCommand.indexCount = mLineIndices.size();
        renderCommand.pipeline = &mLineShader.GetGraphicsPipeline();
        renderCommand.descriptors[0] = &renderer.GetBufferDescriptor();
        renderCommand.descriptorCount = 1;

        renderCommand.pipelineSettings.cullMode = CullMode::None;

        renderer.Submit(renderCommand);

        mLineVertices.clear();
        mLineIndices.clear();
    }
}

bool DebugRenderer::IsEnabled() const
{
    return mEnabled;
}
