#include "ModelImporter.hpp"
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Core/Application.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "Vendor/stb/stb_image.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

std::string GetMeshFromAssimpMesh(const aiMesh *aimesh, const std::string &path, std::unordered_map<const aiMesh *, std::string> &meshMap, uint32_t meshIndex)
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

    std::string identifier = aimesh->mName.C_Str();
    if (aimesh->mName.length == 0)
    {
        identifier = std::format("Untitled: {}", meshIndex);
    }

    std::string meshId = MeshManager::CreateMesh(vertices, indices, identifier);
    MeshManager::GetMeshRef(meshId).EnableSerializing(true);

    return meshId;
}

std::string LoadAssimpTexture(aiTextureType type, const std::string &path, const aiMaterial *aimaterial, const aiScene *aiscene, std::unordered_map<std::string, std::string> &textureMap, bool normalized = true)
{
    std::string result;

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
                result = TextureManager::LoadTexture(fullPath, fullPath, normalized ? ImageFormat::RGBA8 : ImageFormat::RGBA8UNORM);
                Texture &texture = TextureManager::GetTextureRef(result);
                texture.SetName(fullPath);
                texture.SetFilename(fullPath);
                texture.EnableSerializing(true);
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
                    result = TextureManager::CreateTexture(texturePath.C_Str(), data, {width, height}, normalized ? ImageFormat::RGBA8 : ImageFormat::RGBA8UNORM);
                    TextureManager::GetTextureRef(result).SetName(texturePath.C_Str());
                    textureMap[texturePath.C_Str()] = result;
                }
            }
        }
    }

    return result;
}

std::string GetMaterialFromAssimpMaterial(const aiScene *aiscene, const aiMaterial *aimaterial, const std::string &path, std::unordered_map<const aiMaterial *, std::string> &materialMap, std::unordered_map<std::string, std::string> &textureMap, uint32_t materialIndex)
{
    if (materialMap.contains(aimaterial))
    {
        return materialMap[aimaterial];
    }

    CullMode cullMode = CullMode::Front;

    std::string diffuseTextureId = LoadAssimpTexture(aiTextureType_DIFFUSE, path, aimaterial, aiscene, textureMap);
    std::string roughnessTextureId = LoadAssimpTexture(aiTextureType_DIFFUSE_ROUGHNESS, path, aimaterial, aiscene, textureMap, false);
    std::string normalTextureId = LoadAssimpTexture(aiTextureType_NORMALS, path, aimaterial, aiscene, textureMap, false);

    int twoSided = 0;
    aiGetMaterialInteger(aimaterial, AI_MATKEY_TWOSIDED, &twoSided);
    if (twoSided)
    {
        cullMode = CullMode::None;
    }

    aiColor4D color = {1, 1, 1, 1};
    if (diffuseTextureId.size() == 0)
    {
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        unsigned char pixel[4] = {(unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255)};
    }

    Material material;
    material.shader = ShaderManager::GetBuiltinIdentifier().pbr;
    material.albedoTexture = diffuseTextureId;
    material.roughnessTexture = roughnessTextureId;
    material.metallicTexture = roughnessTextureId;
    material.normalTexture = normalTextureId;
    material.cullMode = cullMode;
    material.colorFactor = {color.r, color.g, color.b, color.a};
    material.enableSerializing = true;

    std::string identifier = aimaterial->GetName().C_Str();
    if (aimaterial->GetName().length == 0)
    {
        identifier = std::format("Untitled: {}", materialIndex);
    }

    std::string id = MaterialManager::AddMaterial(material, identifier);
    materialMap[aimaterial] = id;

    return id;
}

void ProcessNode(Scene &scene, const aiScene *aiscene, aiNode *node, const std::string &path, std::unordered_map<const aiMesh *, std::string> &meshMap, std::unordered_map<const aiMaterial *, std::string> &materialMap, std::unordered_map<std::string, std::string> &textureMap)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *aimesh = aiscene->mMeshes[node->mMeshes[i]];
        aiMaterial *aimaterial = aiscene->mMaterials[aimesh->mMaterialIndex];

        std::string meshId = GetMeshFromAssimpMesh(aimesh, path, meshMap, i);
        std::string materialId = GetMaterialFromAssimpMaterial(aiscene, aimaterial, path, materialMap, textureMap, aimesh->mMaterialIndex);

        std::string name = meshId + materialId;
        Entity entity = scene.CreateEntity(name);
        entity.GetComponent<EntityMetadata>().createdFromModel = true;
        entity.GetComponent<EntityMetadata>().enableSerializing = true;
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
    std::unordered_map<const aiMesh *, std::string> meshMap;
    std::unordered_map<const aiMaterial *, std::string> materialMap;
    std::unordered_map<std::string, std::string> textureMap;

    Assimp::Importer importer;
    const aiScene *aiscene = importer.ReadFile(filename.data(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    LOG("importer: {}", importer.GetErrorString());

    std::string basePath = filename.data();
    basePath.erase(basePath.begin() + (int)basePath.find_last_of('/') + 1, basePath.end());

    aiNode *rootNode = aiscene->mRootNode;
    ProcessNode(scene, aiscene, rootNode, basePath, meshMap, materialMap, textureMap);

    importer.FreeScene();
}
