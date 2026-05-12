#include "GameLayer.hpp"
#include "Core/Application.hpp"
#include "EditorLayer.hpp"
#include "EntityComponentSystem.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <memory>

template<>
struct std::formatter<Transform> : std::formatter<std::string>
{
    auto format(const Transform& tranform, std::format_context& context)
    {
        std::format_to(context.out(), "position: {} {} {}\nrotation: {} {} {}\nscale: {} {} {}", tranform.position.x, tranform.position.y, tranform.position.z, tranform.rotation.x, tranform.rotation.y, tranform.rotation.z, tranform.scale.x, tranform.scale.y, tranform.scale.z);
    }
};

struct MeshRendererComponent
{
    std::shared_ptr<StaticMesh> mesh;
    std::shared_ptr<Material> material;
};

void GameLayer::OnAttach() 
{
    GetWindow().Maximize();
    mTarget.Create(GetWindow().GetSize());
    mTarget.TransitionLayout(ImageLayout::General);
    mCameraController.SetCamera(mCamera, GetWindow());

    
    Entity entity = scene.CreateEntity("Entity");
    scene.AddComponent<Transform>(entity);


    const Transform& transform = scene.GetComponent<Transform>(entity);
    std::println("transform: {} {} {}, {} {} {}, {} {} {}", 
                transform.position.x, transform.position.y, transform.position.z,
                transform.rotation.x, transform.rotation.y, transform.rotation.z,
                transform.scale.x, transform.scale.y, transform.scale.z
            );

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

    mesh.SetData(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices));

  
    Assimp::Importer importer;
    const aiScene* aiscene = importer.ReadFile("./Model/city/Untitled.gltf", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    LoadNode(scene, aiscene, aiscene->mRootNode, "./Model/city/");

    commandBuffer.Create();

    CreateMaterial();
}

std::shared_ptr<StaticMesh> GameLayer::GetMeshFromAiMesh(const aiMesh* aimesh)
{
    StaticMesh staticMesh;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    for (int i = 0; i < aimesh->mNumVertices; i++)
    {
        Vertex vertex;
        vertex.position = {aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z};
        vertex.normal = {aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z};
        vertex.uv = {aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y};

        vertices.emplace_back(vertex);
    }

    for (int i = 0; i < aimesh->mNumFaces; i++)
    {
        aiFace face = aimesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return std::make_shared<StaticMesh>(vertices, indices);
}

std::shared_ptr<Material> GameLayer::GetMaterialFromAiMaterial(const aiMaterial* aimaterial, std::string_view basePath)
{
    std::shared_ptr<Material> material = std::make_shared<Material>();

    material->LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
    material->SetCullMode(CullMode::Back);

    aiString path;
    std::string fullPath = basePath.data();
    if(aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path) == aiReturn_SUCCESS)
    {
        fullPath += path.C_Str();
        material->LoadAlbedo(fullPath);
    }
    else
    {
        material->LoadAlbedo("white.png");
    }
    
    material->Create();

    return material;
}

glm::vec3 GetVec3FromAiVec3(const aiVector3t<float> vector)
{
    return {vector.x, vector.y, vector.z};
}

Transform GetTransformFromAiTransform(aiMatrix4x4 aimatrix)
{
    aiVector3t<float> scale, position, rotation;
    aimatrix.Decompose(scale, rotation, position);
    return Transform(GetVec3FromAiVec3(position), GetVec3FromAiVec3(rotation), GetVec3FromAiVec3(scale));
}

void GameLayer::LoadNode(Scene& scene, const aiScene* aiscene, aiNode* node, std::string_view basePath)
{
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        MeshRendererComponent component;

        component.mesh = GetMeshFromAiMesh(aiscene->mMeshes[node->mMeshes[i]]);
        component.material = GetMaterialFromAiMaterial(aiscene->mMaterials[aiscene->mMeshes[node->mMeshes[i]]->mMaterialIndex], basePath);
        Transform transform = GetTransformFromAiTransform(node->mTransformation);

        Entity entity = scene.CreateEntity(node->mName.C_Str());
        entity.AddComponent<Transform>(transform);
        entity.AddComponent<MeshRendererComponent>() = component;
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(scene, aiscene, node->mChildren[i], basePath);
    }
}

void GameLayer::OnUpdate() 
{
    mCameraController.EnableControl(true);
    UpdateCamera();

    GetRenderer().BeginFrame(mTarget, mCamera);
    GetRenderer().Submit(mesh, skyboxMaterial);
    for(auto& [entity, component] : scene.GetEntities<MeshRendererComponent>())
    {
        GetRenderer().Submit(*component.mesh, *component.material, entity.GetComponent<Transform>());
    }

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

    mMaterial.clear();
    mModel.clear();
    mTransforms.clear();
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
    material.SetCullMode(CullMode::None);
    material.Create();

}

void GameLayer::ReloadMaterial() 
{
    vkDeviceWaitIdle(getDevice());

    DestroyMaterial();
    CreateMaterial();
}
