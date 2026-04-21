#include "Maths/Noise.hpp"
#include "Renderer/Mesh.hpp"
#include <Engine.hpp>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include "CameraController.hpp"

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
    void SetBlock(BlockType id, const glm::u8vec3& position)
    {
        assert(position.x <= mSideLength);
        assert(position.z <= mSideLength);
        mChunklets[position.y].blockIds[position.x][position.z] = id;
    }

    void ResetBlock(const glm::u8vec3& position)
    {
        mChunklets[position.y].blockIds[position.x][position.z] = BlockType::Air;
    }

    BlockType GetBlockId(const glm::u8vec3& position)
    {
        return mChunklets[position.y].blockIds[position.x][position.z];
    }

    void SetChunkPosition(const glm::ivec2& position)
    {
        mPosition = position;
    }

    const glm::ivec2& GetChunkPosition()
    {
        return mPosition;
    }

    inline static const int GetHeightLimit()
    {
        return mHeightLimit;
    }

    inline static const int GetSideLength()
    {
        return mSideLength;
    }

private:
    static const uint16_t mHeightLimit = 256;
    static const uint16_t mSideLength = 16;

    Chunklets<mSideLength> mChunklets[mHeightLimit];
    glm::ivec2 mPosition;
};

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

class CoreCraft : public Application
{
    // Chunk mChunk;
    StaticMesh mChunkMesh;
    StaticMesh mCube;

    Material mMaterial;
    Material mSkyboxMaterial;

    Camera mCamera;
    CameraController mController;

    ChunkMeshGenerator mGenerator;

    std::unordered_map<Chunk*, StaticMesh*> mChunks;

    void OnStart() override
    {
        ToggleCursor();
        GetWindowRef().SetFullscreen(true);

        mController.SetCamera(mCamera, GetWindowRef());
        mController.SetSpeed(6);
        mController.SetSensitivity(0.3);
        mCamera.SetPosition({0,80,0});

        std::vector<glm::ivec2> positions = 
        {
        };

        uint32_t renderDistance = 10;

        for (int i = 0; i < renderDistance * renderDistance; i++)
        {
            int x = i % renderDistance;
            int z = i / renderDistance;

            positions.emplace_back(x,z);
        }

        for (glm::ivec2 position : positions)
        {
            Chunk* chunk = new Chunk;
            chunk->SetChunkPosition(position);
            GenerateChunk(*chunk);
            StaticMesh* mesh = new StaticMesh;
            mesh = mGenerator.GenerateMesh(*chunk);
            mChunks[chunk] = mesh;
        }


        SetupMeshes();
        SetupMaterials();
    }

    void OnUpdate() override
    {
        float zoomElapsedTime = mZoomTimer.GetElapsedTime();
        zoomElapsedTime = glm::clamp(zoomElapsedTime, 0.f, 1.f);

        float fov = glm::mix(mCurrentFov, mCamera.GetFov(), 1 - zoomElapsedTime);
        mCamera.SetFov(fov);

        mController.Update();

        
        GetRendererRef().SetCamera(mCamera);
        GetRendererRef().BeginFrame();
        GetRendererRef().DrawMeshWithMaterial(mCube, mSkyboxMaterial, Transform());

        for (auto [chunk, mesh] : mChunks)
        {
            Transform chunkTranform;
            chunkTranform.position = glm::vec3(chunk->GetChunkPosition().x * 16, 0, chunk->GetChunkPosition().y * 16);
            GetRendererRef().DrawMeshWithMaterial(*mesh, mMaterial, chunkTranform);
        }

        GetRendererRef().EndFrame();
    }

    
    void OnEnd() override
    {
        
    }
    
    void GenerateChunk(Chunk& chunk)
    {
        for (int i = 0; i < chunk.GetSideLength() * chunk.GetSideLength(); i++)
        {
            glm::u8vec3 position = {};
            position.x = i % chunk.GetSideLength();
            position.z = (i / chunk.GetSideLength()) % chunk.GetSideLength();

            glm::vec3 worldPosition = glm::vec3(position) + glm::vec3(chunk.GetChunkPosition().x * 16, 0, chunk.GetChunkPosition().y * 16);

            position.y = PerlinNoise(worldPosition / glm::vec3(16.f, 1, 16.f)) + 64;


            chunk.SetBlock(BlockType::Grass, position);

            // chunk.SetBlock(BlockType::Grass, position);

        }

    }

    void OnWindowResize(const glm::uvec2 &size) override
    {
        GetRendererRef().Resize(size);
    }

    void SetupMeshes()
    {
        Vertex vertices[] = 
        {
            // Front
            {{ -0.5, -0.5,  0.5 }, { 0, 0 }, { 0,  0,  1 }},
            {{  0.5, -0.5,  0.5 }, { 1, 0 }, { 0,  0,  1 }},
            {{  0.5,  0.5,  0.5 }, { 1, 1 }, { 0,  0,  1 }},
            {{ -0.5,  0.5,  0.5 }, { 0, 1 }, { 0,  0,  1 }},

            // Back
            {{  0.5, -0.5, -0.5 }, { 0, 0 }, { 0,  0, -1 }},
            {{ -0.5, -0.5, -0.5 }, { 1, 0 }, { 0,  0, -1 }},
            {{ -0.5,  0.5, -0.5 }, { 1, 1 }, { 0,  0, -1 }},
            {{  0.5,  0.5, -0.5 }, { 0, 1 }, { 0,  0, -1 }},

            // Left
            {{ -0.5, -0.5, -0.5 }, { 0, 0 }, { -1, 0,  0 }},
            {{ -0.5, -0.5,  0.5 }, { 1, 0 }, { -1, 0,  0 }},
            {{ -0.5,  0.5,  0.5 }, { 1, 1 }, { -1, 0,  0 }},
            {{ -0.5,  0.5, -0.5 }, { 0, 1 }, { -1, 0,  0 }},

            // Right
            {{  0.5, -0.5,  0.5 }, { 0, 0 }, {  1, 0,  0 }},
            {{  0.5, -0.5, -0.5 }, { 1, 0 }, {  1, 0,  0 }},
            {{  0.5,  0.5, -0.5 }, { 1, 1 }, {  1, 0,  0 }},
            {{  0.5,  0.5,  0.5 }, { 0, 1 }, {  1, 0,  0 }},

            // Top 
            {{ -0.5,  0.5,  0.5 }, { 0, 0 }, {  0, 1,  0 }},
            {{  0.5,  0.5,  0.5 }, { 1, 0 }, {  0, 1,  0 }},
            {{  0.5,  0.5, -0.5 }, { 1, 1 }, {  0, 1,  0 }},
            {{ -0.5,  0.5, -0.5 }, { 0, 1 }, {  0, 1,  0 }},

            // Bottom
            {{ -0.5, -0.5, -0.5 }, { 0, 0 }, {  0, -1, 0 }},
            {{  0.5, -0.5, -0.5 }, { 1, 0 }, {  0, -1, 0 }},
            {{  0.5, -0.5,  0.5 }, { 1, 1 }, {  0, -1, 0 }},
            {{ -0.5, -0.5,  0.5 }, { 0, 1 }, {  0, -1, 0 }},
        };

        uint32_t indices[] = 
        {
             2,  1,  0,    3,  2,  0, // Front
             6,  5,  4,    7,  6,  4, // Back
            10,  9,  8,   11, 10,  8, // Left
            14, 13, 12,   15, 14, 12, // Right
            18, 17, 16,   19, 18, 16, // Top
            22, 21, 20,   23, 22, 20, // Bottom
        };

        mCube.SetData(vertices, sizeof(vertices), indices, sizeof(indices));
    }

    void SetupMaterials()
    {
        mMaterial.LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
        mMaterial.LoadAlbedo("Textures/grassTop.png");
        mMaterial.GetSettingsRef().cullMode = CullMode::Back;
        mMaterial.Create();

        mSkyboxMaterial.LoadShaders("Shaders/skybox.vert.spv", "Shaders/skybox.frag.spv");
        mSkyboxMaterial.GetSettingsRef().depthTestEnable = false;
        mSkyboxMaterial.GetSettingsRef().depthWriteEnable = false;
        mSkyboxMaterial.GetSettingsRef().blendEnable = false;
        mSkyboxMaterial.GetSettingsRef().cullMode = CullMode::Front;
        mSkyboxMaterial.Create();
    }

    float mCurrentFov = 90.f;
    Timer mZoomTimer;

    void OnKeyPress(Key key) override
    {
        if (key == Key::Escape)

        {
            Close();
        }

        if(key == Key::L)
        {
            LOG("Camera Position: {} {} {}", mCamera.GetPosition().x, mCamera.GetPosition().y, mCamera.GetPosition().z);
            LOG("Camera Front: {} {} {}", mCamera.GetFront().x, mCamera.GetFront().y, mCamera.GetFront().z);
        }

        if(key == Key::C)
        {
            mCurrentFov = 30.f;
            mZoomTimer.Start();
        }

        if(key == Key::P)
        {
            CameraType type = (mCamera.GetCameraType() == CameraType::Perspective) ? CameraType::Orthographic : CameraType::Perspective;
            mCamera.SetCameraType(type);
            mCamera.SetNearPlane(0.01f);
            mCamera.SetFarPlane(100.f);

            if(type == CameraType::Orthographic)
            {
                mCamera.SetNearPlane(-100.f);
                mCamera.SetFarPlane(100.f);
            }
        }
    }

    void OnKeyRelease(Key key) override
    {
        if(key == Key::C)
        {
            mCurrentFov = 90.f;
            mZoomTimer.Start();
        }
    }

    void OnScroll(const glm::vec2 &scroll) override
    {
        mCurrentFov += scroll.y;
        mZoomTimer.Start();

        float zoom = mCamera.GetZoom();
        zoom += scroll.y;
        mCamera.SetZoom(zoom);
    }
};

Application* Application::Create()
{
    return new CoreCraft;
}
