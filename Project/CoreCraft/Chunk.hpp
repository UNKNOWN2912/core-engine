#pragma once
#include "Renderer/Mesh.hpp"
#include <glm/glm.hpp>
#include <cstdint>

enum class BlockType : uint16_t
{
    Air = 0,
    Grass,
    Dirt,
    OakLog,
    MaxEnum
};

template<uint16_t sideLength>
struct Chunklets
{
    inline uint16_t GetSideLength() const { return sideLength; }
    BlockType blockIds[sideLength][sideLength];

    Chunklets()
    {
        for (int i = 0; i < GetSideLength() * GetSideLength(); i++)
        {
            int x = i % 16;
            int z = i / 16;
            blockIds[x][z] = BlockType::Air;
        }
    }
};



class Chunk
{
public:
    void SetBlock(BlockType id, const glm::u8vec3& position);
    void ResetBlock(const glm::u8vec3& position);
    BlockType GetBlockId(const glm::u8vec3& position);
    void SetChunkPosition(const glm::ivec2& position);
    const glm::ivec2& GetChunkPosition();
    inline static const int GetHeightLimit();
    inline static const int GetSideLength();

private:
    static const uint16_t mHeightLimit = 256;
    static const uint16_t mSideLength = 16;

    Chunklets<mSideLength> mChunklets[mHeightLimit];
    glm::ivec2 mPosition;
};

inline const int Chunk::GetHeightLimit()
{
    return mHeightLimit;
}

inline const int Chunk::GetSideLength()
{
    return mSideLength;
}
