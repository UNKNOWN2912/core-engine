#pragma once
#include "Utility.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>

enum class LayoutType
{
    Int, UnsignedInt, Float,
    IVec2, UVec2, Vec2,
    IVec3, UVec3, Vec3,
    IVec4, UVec4, Vec4,
};

struct Vertex
{
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;

    Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal): position(position), uv(uv), normal(normal) {}
    Vertex(){}
};


class StaticMesh
{
    public:
        StaticMesh();
        StaticMesh(Vertex* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize);

        void SetData(Vertex* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize);
        // void SetLayout(std::initializer_list<LayoutType> layout);

        void Destroy();
    private:
        friend class Renderer;

        size_t mVertexSize = 0;
        size_t mIndexSize = 0;
        
        Buffer mStagingVertexBuffer;
        Buffer mStagingIndexBuffer;
        
        Buffer mVertexBuffer;
        Buffer mIndexBuffer;

        // std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;
        // VkVertexInputBindingDescription mBindingDescription;
};