#pragma once
#include "Assets/ShaderManager.hpp"
#include "Mesh.hpp"
#include <Renderer/GraphicsPipeline.hpp>
#include <glm/glm.hpp>
#include <string_view>

struct LineVertex
{
    glm::vec3 position;
    glm::vec3 color;
    uint32_t overlay = 0;

    static VertexLayout GetLayout(uint32_t binding, uint32_t startLocation)
    {
        VertexLayout layout;

        layout.attributes.emplace_back(binding, startLocation + 0, offsetof(LineVertex, position), ImageFormat::RGB32);
        layout.attributes.emplace_back(binding, startLocation + 1, offsetof(LineVertex, color), ImageFormat::RGB32);
        layout.attributes.emplace_back(binding, startLocation + 2, offsetof(LineVertex, overlay), ImageFormat::R32U);

        layout.bindings.emplace_back(binding, sizeof(LineVertex), InputRate::Vertex);

        return layout;
    }
};

class DebugRenderer
{
public:
    void Initialize();
    void Terminate();

    void Enable(bool enable);

    void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color, bool overlay = false);
    void DrawWireframe(std::string_view mesh, bool overlay = false);
    void DrawPoint(const glm::vec3 &position, const glm::vec3 &color, bool overlay = false);
    void DrawCuboid(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color, bool overlay = false);
    void DrawCube(const glm::vec3 &position, float size, const glm::vec3 &color, bool overlay = false);
    void DrawFrustrum(const glm::vec3 &start1, const glm::vec3 &end1, const glm::vec3 &start2, const glm::vec3 &end2, const glm::vec3 &color, bool overlay = false);
    void DrawCircleXY(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay = false, uint32_t lineCount = 30);
    void DrawCircleZY(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay = false, uint32_t lineCount = 30);
    void DrawCircleXZ(const glm::vec3 &position, float radius, const glm::vec3 &color, bool overlay = false, uint32_t lineCount = 30);

    void Flush();

    bool IsEnabled() const;

private:
    Shader mLineShader;

    std::vector<LineVertex> mLineVertices;
    std::vector<uint32_t> mLineIndices;

    bool mEnabled = false;

    std::string mDebugLineId;

    Mesh mLineMesh;
    Mesh mPointMesh;
};
