#include "ModelImporter.hpp"
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Core/Application.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "Vendor/stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

MeshID GetMeshFromAssimpMesh(const aiMesh *aimesh, const std::string &path, std::unordered_map<const aiMesh *, MeshID> &meshMap)
{
    if (meshMap.contains(aimesh))
    {
        return meshMap[aimesh];
    }

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(aimesh->mNumVertices);
    indices.reserve(aimesh->mNumFaces * 3l);

    for (int i = 0; i < aimesh->mNumVertices; i++)
    {
        glm::vec3 position = {aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z};
        glm::vec3 normal = {aimesh->mNormals[i].x, aimesh->mNormals[i].y, aimesh->mNormals[i].z};
        glm::vec2 uv = glm::vec2(0);
        glm::vec3 tangent = glm::vec3(0);
        glm::vec3 bitangent = glm::vec3(0);
        if (aimesh->mTextureCoords[0] != nullptr)
        {
            uv = {aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y};
        }
        if (aimesh->HasTangentsAndBitangents())
        {
            tangent = {aimesh->mTangents[i].x, aimesh->mTangents[i].y, aimesh->mTangents[i].z};
            bitangent = {aimesh->mBitangents[i].x, aimesh->mBitangents[i].y, aimesh->mBitangents[i].z};
        }

        vertices.emplace_back(position, uv, normal, tangent, bitangent);
    }

    for (int i = 0; i < aimesh->mNumFaces; i++)
    {
        const aiFace &face = aimesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertices, indices);
    std::string identifier;

    MeshID meshId = MeshManager::AddMesh(mesh);

    if (aimesh->mName.length != 0)
    {
        identifier = aimesh->mName.C_Str();
    }
    else
    {
        identifier = std::to_string((uint64_t)meshId);
    }

    mesh->SetName(identifier);

    meshMap[aimesh] = meshId;

    return meshId;
}

TextureID LoadAssimpTexture(aiTextureType type, const std::string &path, const aiMaterial *aimaterial, const aiScene *aiscene, std::unordered_map<std::string, TextureID> &textureMap, bool normalized = true)
{
    TextureID result = TextureManager::GetInvalidID();

    if (aimaterial->GetTextureCount(type) != 0)
    {
        aiString texturePath;
        aimaterial->GetTexture(type, 0, &texturePath);

        if (textureMap.contains(texturePath.C_Str()))
        {
            result = textureMap[texturePath.C_Str()];
        }
        else
        {
            if (texturePath.C_Str()[0] != '*')
            {
                std::string fullPath = path + texturePath.C_Str();
                result = TextureManager::LoadTexture(fullPath, normalized ? ImageFormat::RGBA8 : ImageFormat::RGBA8UNORM);
                TextureManager::GetTexture(result)->SetName(fullPath);
                textureMap[texturePath.C_Str()] = result;
            }
            else
            {
                int index = atoi(&texturePath.C_Str()[1]);
                const aiTexture *texture = aiscene->mTextures[index];
                if (texture->mHeight == 0)
                {
                    int width = 0, height = 0, channel = 0;
                    stbi_uc *data = stbi_load_from_memory(&texture->pcData->b, (int)texture->mWidth, &width, &height, &channel, 4);
                    result = TextureManager::CreateTexture(data, {width, height}, normalized ? ImageFormat::RGBA8 : ImageFormat::RGBA8UNORM);
                    TextureManager::GetTexture(result)->SetName(texturePath.C_Str());
                    textureMap[texturePath.C_Str()] = result;
                }
            }
        }
    }

    return result;
}

MaterialID GetMaterialFromAssimpMaterial(const aiScene *aiscene, const aiMaterial *aimaterial, const std::string &path, std::unordered_map<const aiMaterial *, MaterialID> &materialMap, std::unordered_map<std::string, TextureID> &textureMap)
{
    if (materialMap.contains(aimaterial))
    {
        return materialMap[aimaterial];
    }

    CullMode cullMode = CullMode::Front;

    TextureID diffuseTextureId = LoadAssimpTexture(aiTextureType_DIFFUSE, path, aimaterial, aiscene, textureMap);
    TextureID roughnessTextureId = LoadAssimpTexture(aiTextureType_DIFFUSE_ROUGHNESS, path, aimaterial, aiscene, textureMap, false);
    TextureID normalTextureId = LoadAssimpTexture(aiTextureType_NORMALS, path, aimaterial, aiscene, textureMap, false);

    int twoSided = 0;
    aiGetMaterialInteger(aimaterial, AI_MATKEY_TWOSIDED, &twoSided);
    if (twoSided)
    {
        cullMode = CullMode::None;
    }

    aiColor4D color = {1, 1, 1, 1};
    if (diffuseTextureId == TextureManager::GetInvalidID())
    {
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        unsigned char pixel[4] = {(unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255)};
        // diffuseTextureId = TextureManager::CreateTexture(pixel, {1, 1}, ImageFormat::RGBA8);
    }

    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->shader = Renderer::GetBasicShaderID();
    material->albedo = diffuseTextureId;
    material->roughness = roughnessTextureId;
    material->metallic = roughnessTextureId;
    material->normal = normalTextureId;
    material->cullMode = cullMode;
    material->colorFactor = {color.r, color.g, color.b, color.a};

    MaterialID id = MaterialManager::AddMaterial(material);
    materialMap[aimaterial] = id;

    return id;
}

void ProcessNode(Scene &scene, const aiScene *aiscene, aiNode *node, const std::string &path, std::unordered_map<const aiMesh *, MeshID> &meshMap, std::unordered_map<const aiMaterial *, MaterialID> &materialMap, std::unordered_map<std::string, TextureID> &textureMap)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *aimesh = aiscene->mMeshes[node->mMeshes[i]];
        aiMaterial *aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

        MeshID meshId = GetMeshFromAssimpMesh(aimesh, path, meshMap);
        MaterialID materialId = GetMaterialFromAssimpMaterial(aiscene, aimaterial, path, materialMap, textureMap);

        std::string name = node->mName.C_Str();
        Entity entity = scene.CreateEntity(name);
        entity.GetComponent<EntityMetadata>().createdFromModel = true;
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
        ProcessNode(scene, aiscene, node->mChildren[i], path, meshMap, materialMap, textureMap);
    }
}

void ModelImporter::Import(std::string_view filename, Scene &scene)
{
    std::unordered_map<const aiMesh *, MeshID> meshMap;
    std::unordered_map<const aiMaterial *, MaterialID> materialMap;
    std::unordered_map<std::string, TextureID> textureMap;

    Assimp::Importer importer;
    const aiScene *aiscene = importer.ReadFile(filename.data(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    LOG("importer: {}", importer.GetErrorString());

    std::string basePath = filename.data();
    basePath.erase(basePath.begin() + basePath.find_last_of('/') + 1, basePath.end());

    aiNode *rootNode = aiscene->mRootNode;
    ProcessNode(scene, aiscene, rootNode, basePath, meshMap, materialMap, textureMap);

    importer.FreeScene();

    scene.AddModelFileImporter(filename.data());
}
