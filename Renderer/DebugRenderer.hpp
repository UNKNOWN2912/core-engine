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
    void DrawRect(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color);
    void DrawCircle(const glm::vec3 &position, float radius, const glm::vec3 &color);

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
