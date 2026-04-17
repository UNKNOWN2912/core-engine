#pragma once
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "Core/Application.hpp"
#include <glm/glm.hpp>

// float perlin(glm::vec3 p);
float combinedPerlin(glm::vec3 st);

struct GrassChunk
{
	InstanceBuffer instanceBuffer;
	int instanceCount = 0;
};

struct TerrainChunk
{
	StaticMesh mesh;
};

class GrassChunkManager
{
	public:
		GrassChunkManager();
		void GenerateChunk(const glm::ivec2& position);
		const std::vector<GrassChunk>& GetChunks() const { return mChunks; }
		void Draw(StaticMesh& mesh, Material& material);
		glm::vec2 GetChunkSize();

		void Clear();

	private:
		std::vector<GrassChunk> mChunks;
		glm::uvec2 mCellCount = {100, 100};
		glm::vec2 mCellSize = {0.2f, 0.2f};
};


class TerrainChunkManager
{
	public:
		void GenerateChunk(glm::ivec2 position);
		const std::vector<TerrainChunk>& GetChunks() const { return mChunks; }
		void Draw(Material& material);
		glm::vec2 GetChunkSize() const;

	private:
		std::vector<TerrainChunk> mChunks; 
		glm::uvec2 mCellCount = {100, 100};
		glm::vec2 mCellSize = {0.2f, 0.2f};

};
