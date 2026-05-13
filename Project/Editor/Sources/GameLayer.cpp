#include "GameLayer.hpp"
#include "Core/Application.hpp"
#include "EditorLayer.hpp"
#include "EntityComponentSystem.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include <memory>
#include <stb_image.h>

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
    const aiScene* aiscene = importer.ReadFile("./Model/bistro/Untitled.glb", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (aiscene == nullptr)
    {
        LOG("importer error: {}", importer.GetErrorString());
    }
    LoadNode(scene, aiscene, aiscene->mRootNode, "./Model/bistro/");

    for (auto& [entity, transform] : scene.GetEntities<Transform>()) 
    {
        transform.scale *= 0.01f;
    }
    
    const aiScene* carScene = importer.ReadFile("./Model/mercedes/Untitled.gltf", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (carScene == nullptr)
    {
        LOG("importer error: {}", importer.GetErrorString());
    }

    // LoadNode(scene, carScene, carScene->mRootNode, "./Model/mercedes/");

    for (auto& [entity, MeshRendererComponent] : scene.GetEntities<MeshRendererComponent>())
    {
        if (entity.GetComponent<EntityMetadata>().name == "Object_500")
        {
            frontRightTier.push_back(entity);
        }
        else if (entity.GetComponent<EntityMetadata>().name == "Object_532")
        {
            frontLeftTier.push_back(entity);
        }
        else if (entity.GetComponent<EntityMetadata>().name == "Object_516")
        {
            backRightTier.push_back(entity);
        }
        else if (entity.GetComponent<EntityMetadata>().name == "Object_485")
        {
            backLeftTier.push_back(entity);
        }
        
    }



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

std::shared_ptr<Material> GameLayer::GetMaterialFromAiMaterial(const aiScene* scene, const aiMaterial* aimaterial, std::string_view basePath)
{
    if(materialMap.Has(aimaterial->GetName().C_Str()))
    {
        return materialMap.Get(aimaterial->GetName().C_Str()); 
    }

    std::shared_ptr<Material> material = std::make_shared<Material>();

    material->LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
    material->SetCullMode(CullMode::Back);

    aiString path;
    std::string fullPath = basePath.data();
    aiReturn result = aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

    std::string p = path.C_Str();
    size_t pos = p.find('\\');
    while (pos != 18446744073709551615u) 
    {
        p[pos] = '/';
        pos = p.find('\\');
    }

    fullPath += p;

    if(textureMap.Has(fullPath))
    {
        material->SetAlbedoTexture(textureMap.Get(fullPath));
    }
    else
    {
        if(result == aiReturn_SUCCESS)
        {
            if(path.C_Str()[0] == '*')
            {
                uint32_t index = atoi(&path.C_Str()[1]);
                aiTexture* texture = scene->mTextures[index];
                if(texture->mHeight == 0)
                {
                    int width, height;
                    const stbi_uc* data = &texture->pcData->b;
                    const stbi_uc* idata = stbi_load_from_memory(data, texture->mWidth, &width, &height, nullptr, 4);

                    material->CreateAlbedo((void*)idata, {width, height});
                }
                else
                {
                    material->CreateAlbedo(texture->pcData, {texture->mWidth, texture->mHeight});
                }
            }
            else
            {
                material->LoadAlbedo(fullPath);
            }
        }
        else
        {
            aiColor4D aicolor;
            aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aicolor);
            
            char r = aicolor.r * 255;
            char g = aicolor.g * 255;
            char b = aicolor.b * 255;
            char a = aicolor.a * 255;

            char color[4] = {r,g,b,a};
            material->CreateAlbedo(color, {1,1});
        }

        textureMap.Set(fullPath, material->GetAlbedo());
    }

    
    material->Create();

    materialMap.Set(aimaterial->GetName().C_Str(), material);

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
        component.material = GetMaterialFromAiMaterial(aiscene, aiscene->mMaterials[aiscene->mMeshes[node->mMeshes[i]]->mMaterialIndex], basePath);
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

    // for (int i = 0; i < frontLeftTier.size(); i++)
    // {
    //     frontLeftTier[i].GetComponent<Transform>().rotation.x += 24;
    // }

    static float speed = 0;

    for (int i = 0; i < frontRightTier.size(); i++)
    {
        float& yRight = frontRightTier[i].GetComponent<Transform>().rotation.y;
        float& yLeft = frontLeftTier[i].GetComponent<Transform>().rotation.y;

        bool pressed = false;

        if (glfwGetKey(GetWindow().GetNativeWindow(), GLFW_KEY_F) == GLFW_PRESS)
        {
            pressed = true;
            yRight = glm::mix(yRight, 35.f, 0.1);
            yLeft = glm::mix(yLeft, 35.f, 0.1);
        }
        if (glfwGetKey(GetWindow().GetNativeWindow(), GLFW_KEY_G) == GLFW_PRESS)
        {
            pressed = true;
            yRight = glm::mix(yRight,-35.f, 0.1);
            yLeft = glm::mix(yLeft,-35.f, 0.1);
        }

        if(!pressed)
        {
            yRight = glm::mix(yRight, 0.f, 0.1);
            yLeft = glm::mix(yLeft, 0.f, 0.1);
        }
    }

    for (int i = 0; i < backRightTier.size(); i++)
    {
        float& xRight = backRightTier[i].GetComponent<Transform>().rotation.x;
        float& xLeft = backLeftTier[i].GetComponent<Transform>().rotation.x;

        if(glfwGetKey(GetWindow().GetNativeWindow(), GLFW_KEY_T) == GLFW_PRESS)
        {
            speed = glm::mix(speed, 256.f, 0.01f);
        }
        else if(glfwGetKey(GetWindow().GetNativeWindow(), GLFW_KEY_T) == GLFW_RELEASE)
        {
            speed = glm::mix(speed, 0.0f, 0.001f);
        }

        xRight = glm::mix(xRight, xRight + (24 * speed), 0.01);
        xLeft = glm::mix(xLeft, xLeft + (24 * speed), 0.01);
    }

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
}

void GameLayer::ReloadMaterial() 
{
    vkDeviceWaitIdle(getDevice());

    DestroyMaterial();
    CreateMaterial();
}
