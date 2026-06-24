#pragma once
#include "Utility.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position = glm::vec3(0);
    glm::vec2 uv = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);

    Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal) : position(position), uv(uv), normal(normal)
    {
    }
    Vertex() = default;
};

class Mesh
{
public:
    Mesh();
    Mesh(void *vertices, size_t vertexSize, uint32_t *indices, size_t indexSize);
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    void SetData(const void *vertices, size_t vertexSize, const uint32_t *indices, size_t indexSize);
    void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);
    bool IsValid() const
    {
        return mIsValid;
    }

    void Destroy();

    const Buffer &GetVertexBuffer() const;
    const Buffer &GetIndexBuffer() const;

    const std::string &GetName() const;
    void SetName(const std::string &name);

private:
    std::string mName;

    friend class Renderer;

    size_t mVertexSize = 0;
    size_t mIndexSize = 0;

    Buffer mStagingVertexBuffer;
    Buffer mStagingIndexBuffer;

    Buffer mVertexBuffer;
    Buffer mIndexBuffer;

    bool mIsValid = false;
};
