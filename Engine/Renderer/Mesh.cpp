#include "Mesh.hpp"
#include "Core/Macro.hpp"
#include <memory.h>

StaticMesh::StaticMesh()
{
    CHROME_TRACE_FUNCTION();
}

StaticMesh::StaticMesh(void* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize)
{
    CHROME_TRACE_FUNCTION();
    SetData(vertices, vertexSize, indices, indexSize);
}

void StaticMesh::SetData(void* vertices, size_t vertexSize, uint32_t* indices, size_t indexSize)
{
    CHROME_TRACE_FUNCTION();
    if(vertexSize != mVertexSize)
    {
        DestroyBuffer(mStagingVertexBuffer);
        DestroyBuffer(mVertexBuffer);
        mStagingVertexBuffer = CreateBuffer(vertexSize, BufferUsage::TransferSource, MemoryProperty::HostCoherent | MemoryProperty::HostVisible);
        mVertexBuffer = CreateBuffer(vertexSize, BufferUsage::VertexBuffer | BufferUsage::TransferDestination, MemoryProperty::DeviceLocal);
        mVertexSize = vertexSize;
    }

    if(indexSize != mIndexSize)
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

void StaticMesh::Destroy() 
{
    CHROME_TRACE_FUNCTION();
    DestroyBuffer(mStagingVertexBuffer);
    DestroyBuffer(mVertexBuffer);
    DestroyBuffer(mStagingIndexBuffer);
    DestroyBuffer(mIndexBuffer);
}
