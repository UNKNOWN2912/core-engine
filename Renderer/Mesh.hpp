#pragma once
#include "Utility.hpp"
#include "Vertex.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct Vertex
{
    glm::vec3 position = glm::vec3(0);
    glm::vec2 uv = glm::vec3(0);
    glm::vec3 normal = glm::vec3(0);

    Vertex(glm::vec3 position, glm::vec2 uv, glm::vec3 normal)
        : position(position), uv(uv), normal(normal)
    {
    }
    Vertex() = default;

    static VertexLayout GetVertexLayout(uint32_t binding, uint32_t startLocation)
    {
        VertexLayout layout;

        layout.attributes.emplace_back(binding, 0 + startLocation, offsetof(Vertex, position), ImageFormat::RGB32);
        layout.attributes.emplace_back(binding, 1 + startLocation, offsetof(Vertex, uv), ImageFormat::RG32);
        layout.attributes.emplace_back(binding, 2 + startLocation, offsetof(Vertex, normal), ImageFormat::RGB32);

        layout.bindings.emplace_back(binding, sizeof(Vertex), InputRate::Vertex);

        return layout;
    }
};

class Mesh
{
public:
    Mesh() = default;
    Mesh(void *vertices, size_t vertexSize, uint32_t *indices, size_t indexSize);
    Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    void SetData(const void *vertices, size_t vertexSize, const uint32_t *indices, size_t indexSize);
    void SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

    void GetMinMax(const glm::mat4 model, glm::vec3 &min, glm::vec3 &max) const
    {
        Vertex *data = (Vertex *)mStagingVertexBuffer.map;

        if (IsStandardMesh() && !IsEmpty())
        {
            min = glm::vec3(FLT_MAX);
            max = glm::vec3(-FLT_MAX);

            for (uint32_t i = 0; i < mVertexBuffer.capacity / sizeof(Vertex); i++)
            {
                glm::vec3 position = model * glm::vec4(data[i].position, 1.f);

                min.x = glm::min(min.x, position.x);
                min.y = glm::min(min.y, position.y);
                min.z = glm::min(min.z, position.z);

                max.x = glm::max(max.x, position.x);
                max.y = glm::max(max.y, position.y);
                max.z = glm::max(max.z, position.z);
            }

            return;
        }

        min = glm::vec3(0);
        max = glm::vec3(0);
    }

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

    size_t GetVertexSize() const
    {
        return mVertexSize;
    }

    size_t GetIndexSize() const
    {
        return mIndexSize;
    }

    bool IsStandardMesh() const
    {
        return mStandardMesh;
    }

    bool IsEmpty() const
    {
        return mStagingVertexBuffer.capacity == 0;
    }

    const glm::vec3 &GetMinVertex() const
    {
        return mMinVertex;
    }

    const glm::vec3 &GetMaxVertex() const
    {
        return mMaxVertex;
    }

    const glm::vec3 &GetCenter() const
    {
        return mCenter;
    }

private:
    std::string mName;

    friend class Renderer;

    size_t mVertexSize = 0;
    size_t mIndexSize = 0;

    Buffer mStagingVertexBuffer;
    Buffer mStagingIndexBuffer;

    Buffer mVertexBuffer;
    Buffer mIndexBuffer;

    glm::vec3 mMinVertex = glm::vec3(FLT_MAX);
    glm::vec3 mMaxVertex = glm::vec3(FLT_MIN);
    glm::vec3 mCenter = glm::vec3(0);

    bool mStandardMesh = false;
    bool mIsValid = false;
};
