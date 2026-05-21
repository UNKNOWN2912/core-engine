#include "ModelImporter.hpp"
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Core/Application.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

MeshID GetMeshFromAssimpMesh(const aiMesh *aimesh, const std::string &path)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(aimesh->mNumVertices);
    indices.reserve(aimesh->mNumFaces * 3);

    for (int i = 0; i < aimesh->mNumVertices; i++)
    {
        glm::vec3 position = {aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z};
        glm::vec3 normal = {aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z};
        glm::vec2 uv = glm::vec2(0);
        if (aimesh->mTextureCoords[0] != nullptr)
            uv = {aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y};

        vertices.emplace_back(position, uv, normal);
    }

    for (int i = 0; i < aimesh->mNumFaces; i++)
    {
        const aiFace &face = aimesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    std::shared_ptr<StaticMesh> mesh = std::make_shared<StaticMesh>(vertices, indices);
    std::string identifier;

    MeshID meshId = MeshManager::AddMesh(mesh);

    if (aimesh->mName.length != 0)
        identifier = aimesh->mName.C_Str();
    else
        identifier = std::to_string((uint64_t)meshId);

    mesh->SetName(identifier);

    return meshId;
}

MaterialID GetMaterialFromAssimpMaterial(const aiMaterial *aimaterial, const std::string &path)
{
    TextureID diffuseTextureId = TextureManager::GetInvalidID();
    if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0)
    {
        aiString texturePath;
        aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        if (texturePath.C_Str()[0] != '*')
        {
            std::string fullPath = path + texturePath.C_Str();
            diffuseTextureId = TextureManager::LoadTexture(fullPath.c_str());
            TextureManager::GetTexture(diffuseTextureId)->SetName(fullPath);
        }
    }

    VertexShaderID vertexShader;
    FragmentShaderID fragmentShader;

    Application::GetInstance()->GetRendererRef().GetBasicShader(vertexShader, fragmentShader);

    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->SetShaders(vertexShader, fragmentShader);
    material->SetAlbedo(diffuseTextureId);
    material->Create();

    MaterialID id = MaterialManager::AddMaterial(material);

    return id;
}

void ProcessNode(Scene &scene, const aiScene *aiscene, aiNode *node, const std::string &path)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *aimesh = aiscene->mMeshes[node->mMeshes[i]];
        aiMaterial *aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

        MeshID meshId = GetMeshFromAssimpMesh(aimesh, path);
        MaterialID materialId = GetMaterialFromAssimpMaterial(aimaterial, path);

        std::string name = MeshManager::GetMesh(meshId)->GetName() + node->mName.C_Str();
        Entity entity = scene.CreateEntity(name);
        if (name.size() == 0)
        {
            entity.GetComponent<EntityMetadata>().name = std::to_string((uint32_t)entity.GetId());
        }

        Transform transform;

        aiVector3D position, rotation, scale;
        node->mTransformation.Decompose(scale, rotation, position);

        transform.position = {position.x, position.y, position.z};
        transform.rotation = {rotation.x, rotation.y, rotation.z};
        transform.scale = {scale.x, scale.y, scale.z};

        entity.AddComponent<Transform>(transform);
        entity.AddComponent<MeshRendererComponent>(meshId, materialId);
    }

    for (int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(scene, aiscene, node->mChildren[i], path);
    }
}

void ModelImporter::Import(std::string_view filename, Scene &scene)
{
    Assimp::Importer importer;
    const aiScene *aiscene = importer.ReadFile(filename.data(), aiProcess_Triangulate | aiProcess_FlipUVs);

    std::string basePath = filename.data();
    basePath.erase(basePath.begin() + basePath.find_last_of('/') + 1, basePath.end());

    aiNode *rootNode = aiscene->mRootNode;
    ProcessNode(scene, aiscene, rootNode, basePath);

    importer.FreeScene();
}