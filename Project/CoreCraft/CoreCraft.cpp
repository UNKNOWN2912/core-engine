#include "Core/Macro.hpp"
#include "Input/Keyboard.hpp"
#include "Maths/Noise.hpp"
#include "MeshGenerator.hpp"
#include "Renderer/Mesh.hpp"
#include <Engine.hpp>
#include <cstdint>
#include <sys/types.h>
#include <vector>
#include "CameraController.hpp"
#include "Chunk.hpp"
#include "glm/ext/vector_float3.hpp"

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
        CHROME_ENABLE_TRACING();
        ToggleCursor();
        // GetWindowRef().SetFullscreen(true);

        mController.SetCamera(mCamera, GetWindowRef());
        mController.SetSpeed(6);
        mController.SetSensitivity(0.3);
        mCamera.SetPosition({0,0,0});

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
        CHROME_DISABLE_TRACING();

    }

    void OnUpdate() override
    {
        float zoomElapsedTime = mZoomTimer.GetElapsedTime();
        zoomElapsedTime = glm::clamp(zoomElapsedTime, 0.f, 1.f);

        float fov = glm::mix(mCurrentFov, mCamera.GetFov(), 1 - zoomElapsedTime);
        mCamera.SetFov(fov);

        mController.Update();

        if(mCamera.GetCameraType() == CameraType::Light)
        {
            mCamera.SetPosition(glm::vec3(100.f));
            mCamera.SetFront(glm::vec3(0));
            mCamera.SetNearPlane(-150.f);
            mCamera.SetFarPlane(150.f);
        }
        
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



            position.y = CombinedPerlin(worldPosition) + 10; 


            chunk.SetBlock(BlockType::Grass, position);

            // chunk.SetBlock(BlockType::Grass, position);

        }

    }

    float CombinedPerlin(glm::vec3 st)
    {
        float l = 2.9;
        float p = 6.3;	
        float result = 0;
        float k = 0.01;

        float a = 10;
        
        for (int i = 0; i < 8; i++)
        {
            float fi = i;
            result += (PerlinNoise(st * glm::pow(l,fi) * k) * a) / pow(p,fi);
        }

        return result;
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
        if (key == Key::Q)
        {
            Close();
        }

        if(key == Key::L)
        {
            LOG("Camera Position: {} {} {}", mCamera.GetPosition().x, mCamera.GetPosition().y, mCamera.GetPosition().z);
            LOG("Camera Front: {} {} {}", mCamera.GetFront().x, mCamera.GetFront().y, mCamera.GetFront().z);
            LOG("Camera Zoom: {}", mCamera.GetZoom());
        }

        if(key == Key::C)
        {
            mCurrentFov = 30.f;
            mZoomTimer.Start();
        }

        if(key == Key::P)
        {
            mCamera.SetCameraType(CameraType::Light);
        }

        if (key == Key::Escape) 
        {
            ToggleCursor();
        }

    }

    void OnWindowMaximize() override
    {
        LOG("Maximize");
    }

    void OnWindowMinimize() override
    {
        LOG("minimize");

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
