#include "ChunkManager.hpp"
#include "Maths/Noise.hpp"


void GrassChunkManager::GenerateChunk(const glm::ivec2& position)
{
	CHROME_TRACE_FUNCTION();

	GrassChunk chunk;
	chunk.position = position;

	glm::ivec2 size = mCellCount;
	glm::vec2 offset = GetChunkSize() * glm::vec2(position);



	glm::mat4* instanceData = new glm::mat4[size.x * size.y];

	for (int i = 0; i < size.x; i++)
	{
		for (int j = 0; j < size.y; j++)
		{
			Transform transform;
			float x = (drand48() - 0.5f) * 2.f;
			float z = (drand48() - 0.5f) * 2.f;
			float r = ((drand48() - 0.5f) * 2.f) * 360.f;

			float s = glm::clamp(drand48(), 0.8, 1.0);

			transform.position = glm::vec3(((mCellSize.x * i) + x) - offset.x, 0, ((mCellSize.y * j) + z) - offset.y);
			transform.scale.y = s;
			transform.position.y = combinedPerlin(transform.position);

			transform.rotation.y = r;

			instanceData[chunk.instanceCount] = transform.GetMatrix();
			chunk.instanceCount++;
		}
	}


	{
		std::lock_guard<std::mutex> lock(bufferWriteMutex);
		chunk.instanceBuffer.SetData(instanceData, sizeof(glm::mat4) * size.x * size.y);
	}

	std::lock_guard<std::mutex> lock(mDrawingMutex);
	mChunks.emplace_back(chunk);

	delete[] instanceData;
}

void GrassChunkManager::Draw(StaticMesh& mesh, Material& material)
{
	CHROME_TRACE_FUNCTION();
	
	std::lock_guard<std::mutex> lock(mDrawingMutex);
	for(GrassChunk& chunk : mChunks)
	{
		Application::GetInstance()->GetRendererRef().DrawMeshWithMaterialInstanced(mesh, material, chunk.instanceBuffer, chunk.instanceCount);
	}

}

glm::vec2 GrassChunkManager::GetChunkSize()
{
	CHROME_TRACE_FUNCTION();
	return mCellSize * glm::vec2(mCellCount);
}

void GrassChunkManager::Clear() 
{
	mChunks.clear();	
}


void TerrainChunkManager::GenerateChunk(glm::ivec2 position)
{
	CHROME_TRACE_FUNCTION();
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	vertices.reserve((mCellCount.x * mCellCount.y * 6));
	indices.reserve(mCellCount.x * mCellCount.y * 6);
	int planeIndex = 0;

	glm::vec2 offset = GetChunkSize() * glm::vec2(position);

	for (int i = 0; i < mCellCount.x; i++)
	{
		for (int j = 0; j < mCellCount.y; j++)
		{
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+1)) - offset.x, 0	, ( mCellSize.y * (j+1))  - offset.y), glm::vec2(1, 0), glm::vec3( 0,  0,  1));
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+1)) - offset.x, 0	, ( mCellSize.y * (j+0))  - offset.y), glm::vec2(1, 1), glm::vec3( 0,  0,  1));
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+0)) - offset.x, 0	, ( mCellSize.y * (j+0))  - offset.y), glm::vec2(0, 1), glm::vec3( 0,  0,  1));
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+0)) - offset.x, 0	, ( mCellSize.y * (j+0))  - offset.y), glm::vec2(0, 1), glm::vec3( 0,  0,  1));
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+0)) - offset.x, 0	, ( mCellSize.y * (j+1))  - offset.y), glm::vec2(0, 0), glm::vec3( 0,  0,  1));
			vertices.emplace_back(glm::vec3(( mCellSize.x * (i+1)) - offset.x, 0	, ( mCellSize.y * (j+1))  - offset.y), glm::vec2(1, 0), glm::vec3( 0,  0,  1));

			(vertices.end() - 6)->position.y = combinedPerlin((vertices.end() - 6)->position);
			(vertices.end() - 5)->position.y = combinedPerlin((vertices.end() - 5)->position);
			(vertices.end() - 4)->position.y = combinedPerlin((vertices.end() - 4)->position);
			(vertices.end() - 3)->position.y = combinedPerlin((vertices.end() - 3)->position);
			(vertices.end() - 2)->position.y = combinedPerlin((vertices.end() - 2)->position);
			(vertices.end() - 1)->position.y = combinedPerlin((vertices.end() - 1)->position);

			glm::vec3 vertex1 = (vertices.end() - 6)->position;			
			glm::vec3 vertex2 = (vertices.end() - 5)->position;			
			glm::vec3 vertex3 = (vertices.end() - 4)->position;

			glm::vec3 vertex4 = (vertices.end() - 3)->position;			
			glm::vec3 vertex5 = (vertices.end() - 2)->position;			
			glm::vec3 vertex6 = (vertices.end() - 1)->position;

			glm::vec3 normal1 = cross(normalize(vertex2 - vertex1), normalize(vertex3 - vertex1));
			glm::vec3 normal2 = cross(normalize(vertex5 - vertex4), normalize(vertex6 - vertex4));


			(vertices.end() - 6)->normal = normal1;
			(vertices.end() - 5)->normal = normal1;
			(vertices.end() - 4)->normal = normal1;
			(vertices.end() - 3)->normal = normal2;
			(vertices.end() - 2)->normal = normal2;
			(vertices.end() - 1)->normal = normal2;

			
			indices.emplace_back(0 + (6 * planeIndex));
			indices.emplace_back(1 + (6 * planeIndex));
			indices.emplace_back(2 + (6 * planeIndex));
			indices.emplace_back(3 + (6 * planeIndex));
			indices.emplace_back(4 + (6 * planeIndex));
			indices.emplace_back(5 + (6 * planeIndex));
			
			planeIndex++;
		}
	}

	TerrainChunk chunk;
	{
		std::lock_guard<std::mutex> lock1(bufferWriteMutex);
		chunk.mesh.SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
	}
	
	std::lock_guard<std::mutex> lock(mDrawingMutex); 
	mChunks.push_back(chunk);
}

void TerrainChunkManager::Draw(Material& material)
{
	CHROME_TRACE_FUNCTION();
	std::lock_guard<std::mutex> lock(mDrawingMutex);
	for(TerrainChunk& chunk : mChunks)
	{
		Application::GetInstance()->GetRendererRef().DrawMeshWithMaterial(chunk.mesh, material, Transform());
	}
}

glm::vec2 TerrainChunkManager::GetChunkSize() const { return mCellSize * glm::vec2(mCellCount); }

float combinedPerlin(glm::vec3 st)
{
	float l = 2.9;
	float p = 6.3;	
	float result = 0;
	float k = 0.01;

	float a = 10;
	
	for (int i = 0; i < 10; i++)
	{
		float fi = i;
		result += (PerlinNoise(st * glm::pow(l,fi) * k) * a) / pow(p,fi);
	}

	return result;
}
