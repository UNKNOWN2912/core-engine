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
    glm::vec3 tangent = glm::vec3(0);
    glm::vec3 bitangent = glm::vec3(0);

    Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal, glm::vec3 tangent, glm::vec3 bitangent)
        : position(position), uv(uv), normal(normal), tangent(tangent), bitangent(bitangent)
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

    void *GetVertexData() const
    {
        return mStagingVertexBuffer.map;
    }

    void *GetIndexData() const
    {
        return mStagingIndexBuffer.map;
    }

    bool IsValid() const;

    void Destroy();

    const Buffer &GetVertexBuffer() const;
    const Buffer &GetIndexBuffer() const;

    const std::string &GetName() const;
    void SetName(const std::string &name);

    static void Initialize();

    void EnableSerializing(bool enable);
    bool IsSerializingEnabled() const;

private:
    std::string mName;

    bool mEnableSerializing = false;

    friend class Renderer;

    size_t mVertexSize = 0;
    size_t mIndexSize = 0;

    Buffer mStagingVertexBuffer;
    Buffer mStagingIndexBuffer;

    Buffer mVertexBuffer;
    Buffer mIndexBuffer;

    bool mIsValid = false;
};
