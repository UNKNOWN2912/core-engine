#pragma once
#include "Chunk.hpp"

class ChunkMeshGenerator
{
    public:
        void SetChunk(Chunk& chunk) { mChunk = &chunk; }

        StaticMesh* GenerateMesh(Chunk& chunk);
    private:
        bool IsAir(glm::u8vec3 position);
        Chunk* mChunk = nullptr;

        void PushTopFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        void PushBottomFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        void PushLeftFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        void PushRightFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        void PushFrontFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
        void PushBackFace(const glm::u8vec3& position, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices);
};
