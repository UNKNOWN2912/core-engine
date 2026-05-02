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
        mStagingVertexBuffer = CreateBuffer(vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        mVertexBuffer = CreateBuffer(vertexSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        mVertexSize = vertexSize;
    }

    if(indexSize != mIndexSize)
    {
        DestroyBuffer(mStagingIndexBuffer);
        DestroyBuffer(mIndexBuffer);
        mStagingIndexBuffer = CreateBuffer(indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        mIndexBuffer = CreateBuffer(indexSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
