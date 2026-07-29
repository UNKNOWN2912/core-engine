#include "Mesh.hpp"
#include "Core/Macro.hpp"
#include <memory.h>

Mesh::Mesh()
{
    CHROME_TRACE_FUNCTION();
}

Mesh::Mesh(void *vertices, size_t vertexSize, uint32_t *indices, size_t indexSize)
{
    CHROME_TRACE_FUNCTION();
    SetData(vertices, vertexSize, indices, indexSize);
}

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
    SetData(vertices, indices);
}

void Mesh::SetData(const void *vertices, size_t vertexSize, const uint32_t *indices, size_t indexSize)
{
    CHROME_TRACE_FUNCTION();
    if (vertexSize != mVertexSize)
    {
        DestroyBuffer(mStagingVertexBuffer);
        DestroyBuffer(mVertexBuffer);
        mStagingVertexBuffer = CreateBuffer(vertexSize, BufferUsage::TransferSource, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
        mVertexBuffer = CreateBuffer(vertexSize, BufferUsage::VertexBuffer | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);
        mVertexSize = vertexSize;
    }

    if (indexSize != mIndexSize)
    {
        DestroyBuffer(mStagingIndexBuffer);
        DestroyBuffer(mIndexBuffer);
        mStagingIndexBuffer = CreateBuffer(indexSize, BufferUsage::TransferSource, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
        mIndexBuffer = CreateBuffer(indexSize, BufferUsage::IndexBuffer | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);
        mIndexSize = indexSize;
    }

    memcpy(mStagingVertexBuffer.map, vertices, mVertexSize);
    memcpy(mStagingIndexBuffer.map, indices, mIndexSize);

    TransferBufferData(mStagingVertexBuffer, mVertexBuffer);
    TransferBufferData(mStagingIndexBuffer, mIndexBuffer);

    mIsValid = true;
}

void Mesh::SetData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices)
{
    SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
}

bool Mesh::IsValid() const
{
    return mIsValid;
}

void Mesh::Destroy()
{
    CHROME_TRACE_FUNCTION();
    DestroyBuffer(mStagingVertexBuffer);
    DestroyBuffer(mVertexBuffer);
    DestroyBuffer(mStagingIndexBuffer);
    DestroyBuffer(mIndexBuffer);
}
const Buffer &Mesh::GetVertexBuffer() const
{
    return mVertexBuffer;
}

const Buffer &Mesh::GetIndexBuffer() const
{
    return mIndexBuffer;
}
const std::string &Mesh::GetName() const
{
    return mName;
}
void Mesh::SetName(const std::string &name)
{
    mName = name;
}

void Mesh::Initialize()
{
}

void Mesh::EnableSerializing(bool enable)
{
    mEnableSerializing = enable;
}

bool Mesh::IsSerializingEnabled() const
{
    return mEnableSerializing;
}
