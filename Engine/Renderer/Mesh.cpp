#include "Mesh.hpp"
#include "Core/Macro.hpp"
#include "GraphicsContext.hpp"
#include <memory.h>

size_t vertexLayoutByteSize[] = 
{
    4, 4, 4,
    8, 8, 8,
    12, 12, 12,
    16, 16, 16,
};

VkFormat vertexLayoutFormat[] = 
{
    VK_FORMAT_R32_SINT,          VK_FORMAT_R32_UINT,         VK_FORMAT_R32_SFLOAT,         
    VK_FORMAT_R32G32_SINT,       VK_FORMAT_R32G32_UINT,      VK_FORMAT_R32G32_SFLOAT,      
    VK_FORMAT_R32G32B32_SINT,    VK_FORMAT_R32G32B32_UINT,   VK_FORMAT_R32G32B32_SFLOAT,   
    VK_FORMAT_R32G32B32A32_SINT, VK_FORMAT_R32G32B32A32_UINT,VK_FORMAT_R32G32B32A32_SFLOAT,
};

StaticMesh::StaticMesh()
{
    CHROME_TRACE_FUNCTION();
    // mBindingDescription.binding = 0;
    // mBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
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

// void StaticMesh::SetLayout(std::initializer_list<LayoutType> layout)
// {
//     uint32_t i = 0;
//     uint32_t offset = 0;
//     for (LayoutType type : layout) 
//     {
//         VkVertexInputAttributeDescription attribute = 
//         {
//             .location = i,
//             .binding = 0,
//             .format = vertexLayoutFormat[(uint32_t)type],
//             .offset = offset,
//         };

//         mAttributeDescriptions.push_back(attribute);

//         offset += vertexLayoutByteSize[(uint32_t)type];
//         i++;
//     }

//     mBindingDescription.stride = offset;
// }
void StaticMesh::Destroy() 
{
    CHROME_TRACE_FUNCTION();
    DestroyBuffer(mStagingVertexBuffer);
    DestroyBuffer(mVertexBuffer);
    DestroyBuffer(mStagingIndexBuffer);
    DestroyBuffer(mIndexBuffer);
}
