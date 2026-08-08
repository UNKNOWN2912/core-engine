#pragma once
#include "Mesh.hpp"
#include <Renderer/GraphicsPipeline.hpp>
#include <glm/glm.hpp>
#include <string_view>

class DebugRenderer
{
public:
    void Initialize();
    void Terminate();

    void Enable(bool enable);

    void DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
    void DrawWireframe(std::string_view mesh);
    void DrawPoint(const glm::vec3 &position, const glm::vec3 &color);
    void DrawCuboid(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
    void DrawCube(const glm::vec3 &position, float size, const glm::vec3 &color);
    void DrawFrustrum(const glm::vec3 &start1, const glm::vec3 &end1, const glm::vec3 &start2, const glm::vec3 &end2, const glm::vec3 &color);
    void DrawCircleXY(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount = 30);
    void DrawCircleZY(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount = 30);
    void DrawCircleXZ(const glm::vec3 &position, float radius, const glm::vec3 &color, uint32_t lineCount = 30);

    void Flush();

private:
    struct LineVertex
    {
        glm::vec3 position;
        glm::vec3 color;
    };

    std::vector<LineVertex> mLineVertices;
    std::vector<uint32_t> mLineIndices;

    GraphicsPipeline mLinePipeline;
    bool mEnabled = false;

    std::string mDebugLineId;

    Mesh mLineMesh;
    Mesh mPointMesh;
};
