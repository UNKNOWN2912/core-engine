#include "DebugRenderer.hpp"
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
void DebugRenderer::DrawRect(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
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
void DebugRenderer::DrawCircle(const glm::vec3 &position, float radius, const glm::vec3 &color)
{
    if (!mEnabled)
        return;
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

        Renderer::Submit(renderCommand);

        mLineVertices.clear();
        mLineIndices.clear();
    }
}
