#include "MeshGenerator.hpp"
#include "Core/Macro.hpp"



StaticMesh* ChunkMeshGenerator::GenerateMesh(Chunk& chunk)
{
    SetChunk(chunk);

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < mChunk->GetHeightLimit() * mChunk->GetSideLength() * mChunk->GetSideLength(); i++)
    {
        glm::u8vec3 position;
        position.x = i % mChunk->GetSideLength();
        position.z = (i / mChunk->GetSideLength()) % mChunk->GetSideLength();
        position.y = (i / (mChunk->GetSideLength() * mChunk->GetSideLength())) % mChunk->GetHeightLimit();


        if(IsAir(position))
            continue;

        if(IsAir(position + glm::u8vec3( 0, 1, 0)))
        {
            PushTopFace(position, vertices, indices);
        }
        if(IsAir(position + glm::u8vec3( 0,-1, 0)))
        {
            PushBottomFace(position, vertices, indices);
        }
        if(IsAir(position + glm::u8vec3(-1, 0, 0)))
        {
            PushLeftFace(position, vertices, indices);
        }
        if(IsAir(position + glm::u8vec3( 1, 0, 0)))
        {
            PushRightFace(position, vertices, indices);
        }
        if(IsAir(position + glm::u8vec3( 0, 0, 1)))
        {
            PushFrontFace(position, vertices, indices);
        }
        if(IsAir(position + glm::u8vec3( 0, 0,-1)))
        {
            PushBackFace(position, vertices, indices);
        }
    }

    StaticMesh* mesh = new StaticMesh;
    mesh->SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
    return mesh;
}

bool ChunkMeshGenerator::IsAir(glm::u8vec3 position) 
{
    glm::u8 s = -1;

    if(position == glm::u8vec3(16,0,0))
    {
        LOG("Hello");
    }

    int sideLength = mChunk->GetSideLength();
    int heightLimit = mChunk->GetHeightLimit();

    if(position.x >= sideLength || position.x == 255)
        return true;    
    if(position.y > heightLimit)
        return true;
    if(position.z >= sideLength || position.y == 255)
        return true;

    return mChunk->GetBlockId(position) == BlockType::Air;
}

void ChunkMeshGenerator::PushTopFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Top 
        {{ -0.5 + position.x,  0.5 + position.y,  0.5 + position.z }, { 0, 0 }, {  0, 1,  0 }},
        {{  0.5 + position.x,  0.5 + position.y,  0.5 + position.z }, { 1, 0 }, {  0, 1,  0 }},
        {{  0.5 + position.x,  0.5 + position.y, -0.5 + position.z }, { 1, 1 }, {  0, 1,  0 }},
        {{ -0.5 + position.x,  0.5 + position.y, -0.5 + position.z }, { 0, 1 }, {  0, 1,  0 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
void ChunkMeshGenerator::PushBottomFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Bottom
        {{ -0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 0, 0 }, {  0, -1, 0 }},
        {{  0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 1, 0 }, {  0, -1, 0 }},
        {{  0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 1, 1 }, {  0, -1, 0 }},
        {{ -0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 0, 1 }, {  0, -1, 0 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
void ChunkMeshGenerator::PushLeftFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Left
        {{ -0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 0, 0 }, { -1, 0,  0 }},
        {{ -0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 1, 0 }, { -1, 0,  0 }},
        {{ -0.5 + position.x,  0.5 + position.y,  0.5 + position.z}, { 1, 1 }, { -1, 0,  0 }},
        {{ -0.5 + position.x,  0.5 + position.y, -0.5 + position.z}, { 0, 1 }, { -1, 0,  0 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
void ChunkMeshGenerator::PushRightFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Right
        {{  0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 0, 0 }, {  1, 0,  0 }},
        {{  0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 1, 0 }, {  1, 0,  0 }},
        {{  0.5 + position.x,  0.5 + position.y, -0.5 + position.z}, { 1, 1 }, {  1, 0,  0 }},
        {{  0.5 + position.x,  0.5 + position.y,  0.5 + position.z}, { 0, 1 }, {  1, 0,  0 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
void ChunkMeshGenerator::PushFrontFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Front
        {{ -0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 0, 0 }, { 0,  0,  1 }},
        {{  0.5 + position.x, -0.5 + position.y,  0.5 + position.z}, { 1, 0 }, { 0,  0,  1 }},
        {{  0.5 + position.x,  0.5 + position.y,  0.5 + position.z}, { 1, 1 }, { 0,  0,  1 }},
        {{ -0.5 + position.x,  0.5 + position.y,  0.5 + position.z}, { 0, 1 }, { 0,  0,  1 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
void ChunkMeshGenerator::PushBackFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices)
{
    uint32_t indexOffset = (indices.size() == 0) ? 0 : (indices[indices.size() - 3]) + 1;

    Vertex faceVertices[] = 
    {
        // Back
        {{  0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 0, 0 }, { 0,  0, -1 }},
        {{ -0.5 + position.x, -0.5 + position.y, -0.5 + position.z}, { 1, 0 }, { 0,  0, -1 }},
        {{ -0.5 + position.x,  0.5 + position.y, -0.5 + position.z}, { 1, 1 }, { 0,  0, -1 }},
        {{  0.5 + position.x,  0.5 + position.y, -0.5 + position.z}, { 0, 1 }, { 0,  0, -1 }},
    };
    uint32_t faceIndices[] = 
    {
        2 + (indexOffset),  1 + (indexOffset),  0 + (indexOffset),    
        3 + (indexOffset),  2 + (indexOffset),  0 + (indexOffset),
    };

    vertices.insert(vertices.end(), std::begin(faceVertices), std::end(faceVertices));
    indices.insert(indices.end(), std::begin(faceIndices), std::end(faceIndices));
}
