#include "ChunkManager.hpp"

void GrassChunkManager::GenerateChunk(const glm::ivec2& position)
{
	GrassChunk chunk;
	glm::ivec2 size = mCellCount;
	glm::vec2 offset = GetChunkSize() * glm::vec2(position);

	std::vector<glm::mat4> instanceData;
	instanceData.reserve(size.x * size.y);
	

	for (int i = 0; i < size.x; i++)
	{
		for (int j = 0; j < size.y; j++)
		{
			Transform transform;
			float x = (drand48() - 0.5f) * 2.f;
			float z = (drand48() - 0.5f) * 2.f;
			float r = ((drand48() - 0.5f) * 2.f) * 360.f;

			transform.position = glm::vec3(((mCellSize.x * i) + x) - offset.x, 0, ((mCellSize.y * j) + z) - offset.y);
			transform.position.y = combinedPerlin(transform.position);

			transform.rotation.y = r;

			instanceData.emplace_back(transform.GetMatrix());
			chunk.instanceCount++;
		}
	}

	chunk.instanceBuffer.SetData(instanceData.data(), sizeof(glm::mat4) * instanceData.size());

	mChunks.push_back(chunk);
}

void GrassChunkManager::Draw(StaticMesh& mesh, Material& material)
{
	for(GrassChunk& chunk : mChunks)
	{
		Application::GetInstance()->GetRendererRef().DrawMeshWithMaterialInstanced(mesh, material, chunk.instanceBuffer, chunk.instanceCount);
	}

}

glm::vec2 GrassChunkManager::GetChunkSize()
{
	return mCellSize * glm::vec2(mCellCount);
}

void GrassChunkManager::Clear() 
{
	mChunks.clear();	
}


void TerrainChunkManager::GenerateChunk(glm::ivec2 position)
{
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
	chunk.mesh.SetData(vertices.data(), sizeof(Vertex) * vertices.size(), indices.data(), sizeof(uint32_t) * indices.size());
	
	mChunks.push_back(chunk);
}

void TerrainChunkManager::Draw(Material& material)
{
	for(TerrainChunk& chunk : mChunks)
	{
		Application::GetInstance()->GetRendererRef().DrawMeshWithMaterial(chunk.mesh, material, Transform());
	}
}

glm::vec2 TerrainChunkManager::GetChunkSize() const { return mCellSize * glm::vec2(mCellCount); }

float perlin(glm::vec3 p) 
{
    auto fade = [](float t) 
    {
        return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    };

    auto grad = [](int hash, glm::vec3 p) 
    {
        switch (hash & 15) 
        {
            case  0: return  p.x + p.y;
            case  1: return -p.x + p.y;
            case  2: return  p.x - p.y;
            case  3: return -p.x - p.y;
            case  4: return  p.x + p.z;
            case  5: return -p.x + p.z;
            case  6: return  p.x - p.z;
            case  7: return -p.x - p.z;
            case  8: return  p.y + p.z;
            case  9: return -p.y + p.z;
            case 10: return  p.y - p.z;
            case 11: return -p.y - p.z;
            case 12: return  p.y + p.x;
            case 13: return -p.y + p.z;
            case 14: return  p.y - p.x;
            case 15: return -p.y - p.z;
            default: return 0.0f;
        }
    };

    static const int perm[512] = 
    {
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,
        69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,
        252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,
        171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,
        122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,
        161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
        159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,
        147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
        246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,
        121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
        128,195,78,66,215,61,156,180,
        // duplicated
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,
        69,142,8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,
        252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,
        171,168,68,175,74,165,71,134,139,48,27,166,77,146,158,231,83,111,229,
        122,60,211,133,230,220,105,92,41,55,46,245,40,244,102,143,54,65,25,63,
        161,1,216,80,73,209,76,132,187,208,89,18,169,200,196,135,130,116,188,
        159,86,164,100,109,198,173,186,3,64,52,217,226,250,124,123,5,202,38,
        147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
        223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
        172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,
        246,97,228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,
        235,249,14,239,107,49,192,214,31,181,199,106,157,184,84,204,176,115,
        121,50,45,127,4,150,254,138,236,205,93,222,114,67,29,24,72,243,141,
        128,195,78,66,215,61,156,180
    };

    glm::ivec3 i = glm::ivec3(glm::floor(p)) & 255;
    glm::vec3  f = glm::fract(p);
    glm::vec3  u = glm::vec3(fade(f.x), fade(f.y), fade(f.z));

    int aaa = perm[perm[perm[i.x    ] + i.y    ] + i.z    ];
    int aba = perm[perm[perm[i.x    ] + i.y + 1] + i.z    ];
    int aab = perm[perm[perm[i.x    ] + i.y    ] + i.z + 1];
    int abb = perm[perm[perm[i.x    ] + i.y + 1] + i.z + 1];
    int baa = perm[perm[perm[i.x + 1] + i.y    ] + i.z    ];
    int bba = perm[perm[perm[i.x + 1] + i.y + 1] + i.z    ];
    int bab = perm[perm[perm[i.x + 1] + i.y    ] + i.z + 1];
    int bbb = perm[perm[perm[i.x + 1] + i.y + 1] + i.z + 1];

    glm::vec3 f1 = f - glm::vec3(1.0f);

    float x1 = glm::mix(grad(aaa, f),                        grad(baa, {f1.x, f.y,  f.z }),  u.x);
    float x2 = glm::mix(grad(aba, {f.x, f1.y, f.z }),        grad(bba, {f1.x, f1.y, f.z }),  u.x);
    float x3 = glm::mix(grad(aab, {f.x, f.y,  f1.z}),        grad(bab, {f1.x, f.y,  f1.z}), u.x);
    float x4 = glm::mix(grad(abb, {f.x, f1.y, f1.z}),        grad(bbb, {f1.x, f1.y, f1.z}), u.x);

    float y1 = glm::mix(x1, x2, u.y);
    float y2 = glm::mix(x3, x4, u.y);

    return glm::mix(y1, y2, u.z);
}

float sin2d(glm::vec3 st)
{
	return sin(st.x + st.z);
}

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
		result += (perlin(st * glm::pow(l,fi) * k) * a) / pow(p,fi);
	}

	return result;
}
