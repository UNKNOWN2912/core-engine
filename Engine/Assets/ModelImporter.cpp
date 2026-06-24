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
        if (aimesh->mTextureCoords[0] != nullptr)
        {
            uv = {aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y};
        }

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

MaterialID GetMaterialFromAssimpMaterial(const aiScene *aiscene, const aiMaterial *aimaterial, const std::string &path, std::unordered_map<const aiMaterial *, MaterialID> &materialMap, std::unordered_map<std::string, TextureID> &textureMap)
{
    if (materialMap.contains(aimaterial))
    {
        return materialMap[aimaterial];
    }

    TextureID diffuseTextureId = TextureManager::GetInvalidID();
    CullMode cullMode = CullMode::Front;

    if (aimaterial->GetTextureCount(aiTextureType_DIFFUSE) != 0)
    {
        aiString texturePath;
        aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

        if (textureMap.contains(texturePath.C_Str()))
        {
            diffuseTextureId = textureMap[texturePath.C_Str()];
        }
        else
        {
            if (texturePath.C_Str()[0] != '*')
            {
                std::string fullPath = path + texturePath.C_Str();
                diffuseTextureId = TextureManager::LoadTexture(fullPath);
                TextureManager::GetTexture(diffuseTextureId)->SetName(fullPath);
                textureMap[texturePath.C_Str()] = diffuseTextureId;
            }
            else
            {
                int index = atoi(&texturePath.C_Str()[1]);
                const aiTexture *texture = aiscene->mTextures[index];
                if (texture->mHeight == 0)
                {
                    int width = 0, height = 0, channel = 0;
                    stbi_uc *data = stbi_load_from_memory(&texture->pcData->b, (int)texture->mWidth, &width, &height, &channel, 4);
                    diffuseTextureId = TextureManager::CreateTexture(data, {width, height}, ImageFormat::RGBA8);
                    TextureManager::GetTexture(diffuseTextureId)->SetName(texturePath.C_Str());
                    textureMap[texturePath.C_Str()] = diffuseTextureId;
                }
            }
        }
    }
    else
    {
        aiColor4D color;
        aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        unsigned char pixel[4] = {(unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255)};
        diffuseTextureId = TextureManager::CreateTexture(pixel, {1, 1}, ImageFormat::RGBA8);

        int twoSided = 0;
        aiGetMaterialInteger(aimaterial, AI_MATKEY_TWOSIDED, &twoSided);
        if (twoSided)
        {
            cullMode = CullMode::None;
        }
    }

    VertexShaderID vertexShader = ShaderManager::GetInvalidVertexShaderID();
    FragmentShaderID fragmentShader = ShaderManager::GetInvalidFragmentShaderID();

    Renderer::GetBasicShader(vertexShader, fragmentShader);

    std::shared_ptr<Material> material = std::make_shared<Material>();
    material->vertexShader = vertexShader;
    material->fragmentShader = fragmentShader;
    material->albedo = diffuseTextureId;
    material->cullMode = cullMode;
    Renderer::RegisterMaterial(*material);

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
        ProcessNode(scene, aiscene, node->mChildren[i], path, meshMap, materialMap, textureMap);
    }
}

void ModelImporter::Import(std::string_view filename, Scene &scene)
{
    std::unordered_map<const aiMesh *, MeshID> meshMap;
    std::unordered_map<const aiMaterial *, MaterialID> materialMap;
    std::unordered_map<std::string, TextureID> textureMap;

    Assimp::Importer importer;
    const aiScene *aiscene = importer.ReadFile(filename.data(), aiProcess_Triangulate | aiProcess_FlipUVs);
    LOG("importer: {}", importer.GetErrorString());

    std::string basePath = filename.data();
    basePath.erase(basePath.begin() + basePath.find_last_of('/') + 1, basePath.end());

    aiNode *rootNode = aiscene->mRootNode;
    ProcessNode(scene, aiscene, rootNode, basePath, meshMap, materialMap, textureMap);

    importer.FreeScene();
}
