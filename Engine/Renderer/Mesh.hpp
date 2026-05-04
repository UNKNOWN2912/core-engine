#pragma once
#include "Utility.hpp"
#include <cstddef>
#include <cstdint>
#include <vector>


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
        StaticMesh(void* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize);

        void SetData(void* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize);
        bool IsValid() const { return mIsValid; }

        void Destroy();

        const Buffer& GetVertexBuffer() const { return mVertexBuffer; }
        const Buffer& GetIndexBuffer() const { return mIndexBuffer; }
    private:
        friend class Renderer;

        size_t mVertexSize = 0;
        size_t mIndexSize = 0;
        
        Buffer mStagingVertexBuffer;
        Buffer mStagingIndexBuffer;
        
        Buffer mVertexBuffer;
        Buffer mIndexBuffer;

        bool mIsValid = false;
};