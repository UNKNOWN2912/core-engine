#include "GameLayer.hpp"
#include "Assets/ModelImporter.hpp"
#include "EditorLayer.hpp"
#include "EntityComponentSystem/Component.hpp"
#include <Engine.hpp>
#include <stb_image.h>

template <>
struct std::formatter<Transform> : std::formatter<std::string>
{
    auto format(const Transform &tranform, std::format_context &context)
    {
        std::format_to(context.out(), "position: {} {} {}\nrotation: {} {} {}\nscale: {} {} {}", tranform.position.x, tranform.position.y, tranform.position.z, tranform.rotation.x, tranform.rotation.y, tranform.rotation.z, tranform.scale.x, tranform.scale.y, tranform.scale.z);
    }
};

void GameLayer::OnAttach()
{
    GetWindow().Maximize();
    mTarget.Create(GetWindow().GetSize());
    mTarget.TransitionLayout(ImageLayout::General);
    mCameraController.SetCamera(mCamera, GetWindow());

    VertexShaderID vertexShaderID = ShaderManager::LoadVertexShader("Shaders/shader.vert.spv");
    FragmentShaderID fragmentShaderID = ShaderManager::LoadFragmentShader("Shaders/shader.frag.spv");

    GetRenderer().SetBasicShader(vertexShaderID, fragmentShaderID);

    Entity entity = scene.CreateEntity("Entity");
    scene.AddComponent<Transform>(entity);

    ModelImporter importer;
    importer.Import("./Model/sponza/Sponza.gltf", scene);

    const Transform &transform = scene.GetComponent<Transform>(entity);
    std::println("transform: {} {} {}, {} {} {}, {} {} {}",
                 transform.position.x, transform.position.y, transform.position.z,
                 transform.rotation.x, transform.rotation.y, transform.rotation.z,
                 transform.scale.x, transform.scale.y, transform.scale.z);

    // clang-format off
    
    Vertex cubeVertices[] = 
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


    uint32_t cubeIndices[] = 
    {
         2,  1,  0,    3,  2,  0,  // Front
         6,  5,  4,    7,  6,  4,  // Back
        10,  9,  8,   11, 10,  8, // Left
        14, 13, 12,   15, 14, 12, // Right
        18, 17, 16,   19, 18, 16, // Top
        22, 21, 20,   23, 22, 20, // Bottom
    };

    // clang-format on

    mesh.SetData(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));

    CreateMaterial();
}

void GameLayer::OnUpdate()
{
    mCameraController.EnableControl(true);
    UpdateCamera();

    GetRenderer().BeginFrame(mTarget, mCamera);
    GetRenderer().Submit(mesh, skyboxMaterial);

    for (auto &[entity, component] : scene.GetEntities<MeshRendererComponent>())
    {
        std::shared_ptr<StaticMesh> mesh = MeshManager::GetMesh(component.mesh);
        std::shared_ptr<Material> material = MaterialManager::GetMaterial(component.material);
        GetRenderer().Submit(*mesh, *material, entity.GetComponent<Transform>());
    }

    GetRenderer().EndFrame();
}

void GameLayer::OnDetach()
{
}

void GameLayer::UpdateCamera()
{
    mCameraController.Update();
    mCamera.Calculate();
    mCamera = GetLayer<EditorLayer>().GetEditorCamera();
}

void GameLayer::DestroyMaterial()
{
    skyboxMaterial.Destroy();
}

void GameLayer::CreateMaterial()
{
    VertexShaderID skyboxVertexId = ShaderManager::LoadVertexShader("Shaders/skybox.vert.spv");
    FragmentShaderID skyboxFragmentId = ShaderManager::LoadFragmentShader("Shaders/skybox.frag.spv");

    skyboxMaterial.SetShaders(skyboxVertexId, skyboxFragmentId);
    skyboxMaterial.SetCullMode(CullMode::None);
    skyboxMaterial.EnableDepthTestEnable(false);
    skyboxMaterial.EnableDepthWriteEnable(false);
    skyboxMaterial.Create();
}

void GameLayer::ReloadMaterial()
{
    vkDeviceWaitIdle(getDevice());

    DestroyMaterial();
    CreateMaterial();
}
