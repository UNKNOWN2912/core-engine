#include "Chunk.hpp"

void Chunk::SetBlock(BlockType id, const glm::u8vec3& position)
{
    assert(position.x <= mSideLength);
    assert(position.z <= mSideLength);
    mChunklets[position.y].blockIds[position.x][position.z] = id;
}

void Chunk::ResetBlock(const glm::u8vec3& position)
{
    mChunklets[position.y].blockIds[position.x][position.z] = BlockType::Air;
}

BlockType Chunk::GetBlockId(const glm::u8vec3& position)
{
    return mChunklets[position.y].blockIds[position.x][position.z];
}

void Chunk::SetChunkPosition(const glm::ivec2& position)
{
    mPosition = position;
}

const glm::ivec2& Chunk::GetChunkPosition()
{
    return mPosition;
}

