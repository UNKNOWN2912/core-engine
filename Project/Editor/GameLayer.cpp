#include "GameLayer.hpp"
#include "EditorLayer.hpp"

void GameLayer::OnAttach() 
{
    GetWindow().Maximize();
    mTarget.Create(GetWindow().GetSize());
    mTarget.TransitionLayout(ImageLayout::General);
    mCameraController.SetCamera(mCamera, GetWindow());


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
         2,  1,  0,    3,  2,  0,  // Front
         6,  5,  4,    7,  6,  4,  // Back
        10,  9,  8,   11, 10,  8, // Left
        14, 13, 12,   15, 14, 12, // Right
        18, 17, 16,   19, 18, 16, // Top
        22, 21, 20,   23, 22, 20, // Bottom
    };

    mesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));

    CreateMaterial();
}

void GameLayer::OnUpdate() 
{
    mCameraController.EnableControl(true);
    UpdateCamera();

    GetRenderer().BeginFrame(mTarget, mCamera);

    GetRenderer().Submit(mesh, skyboxMaterial);
    GetRenderer().Submit(mesh, material);

    GetRenderer().EndFrame();
}

void GameLayer::OnDetach() {}

void GameLayer::UpdateCamera() 
{
    mCameraController.Update();
    mCamera.Calculate();
    mCamera = GetLayer<EditorLayer>().GetEditorCamera();
}

void GameLayer::DestroyMaterial() 
{
    skyboxMaterial.Destroy();
    material.Destroy();
}

void GameLayer::CreateMaterial() 
{
    skyboxMaterial.LoadShaders("Shaders/skybox.vert.spv",
                                "Shaders/skybox.frag.spv");
    skyboxMaterial.SetCullMode(CullMode::None);
    skyboxMaterial.EnableDepthTestEnable(false);
    skyboxMaterial.EnableDepthWriteEnable(false);
    skyboxMaterial.Create();

    material.LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
    material.LoadAlbedo("Textures/sample.png");
    material.SetCullMode(CullMode::Front);
    material.Create();

    commandBuffer.Create();
}
