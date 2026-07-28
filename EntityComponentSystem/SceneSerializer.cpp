#include "SceneSerializer.hpp"
#include "json.hpp"
#include <Engine.hpp>
#include <fstream>

using namespace nlohmann;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////                 Exporter                  ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

json GetJsonFromMaterial(const Material &material)
{
    json json;

    json["albedoTexture"] = material.albedoTexture;
    json["metallicTexture"] = material.metallicTexture;
    json["roughnessTexture"] = material.roughnessTexture;
    json["normalTexture"] = material.normalTexture;
    json["shader"] = material.shader;
    json["cullMode"] = material.cullMode;
    json["colorFactor"] = {material.colorFactor.r, material.colorFactor.g, material.colorFactor.b, material.colorFactor.a};
    json["roughnessFactor"] = material.roughnessFactor;
    json["metallicFactor"] = material.metallicFactor;
    json["indexOfRefraction"] = material.indexOfRefraction;
    json["enableDepthWrite"] = material.enableDepthWrite;
    json["enableDepthTest"] = material.enableDepthTest;
    json["enableBlending"] = material.enableBlending;
    json["name"] = material.name;

    return json;
}

json GetJsonFromShader(const Shader &shader)
{
    json json;
    json["vertex"] = shader.vertexPath;
    json["fragment"] = shader.fragmentPath;
    json["geometry"] = shader.geometryPath;
    json["tessellation"] = shader.tessellationPath;
    json["autoCreatePipeline"] = shader.createRendererObjects;
    return json;
}

json GetJsonFromTexture(const Texture &texture)
{
    json json;
    json["filename"] = texture.GetFilename();
    return json;
}

json GetJsonFromMesh(const Mesh &mesh, std::string_view vertexDataStorage, std::string_view indexDataStorage, size_t vertexOffset, uint32_t vertexCount, size_t indexOffset, uint32_t indexCount)
{
    json json;
    json["name"] = mesh.GetName();
    json["vertexFile"] = vertexDataStorage;
    json["indexFile"] = indexDataStorage;
    json["vertexOffset"] = vertexOffset;
    json["indexOffset"] = indexOffset;
    json["vertexCount"] = vertexCount;
    json["indexCount"] = indexCount;

    return json;
}

json GetJsonFromEntityMetadata(const EntityMetadata &metadata)
{
    json json;
    json["name"] = metadata.name;
    return json;
}

json GetJsonFromMeshRenderer(const MeshRendererComponent &meshRenderer)
{
    json json;
    json["mesh"] = meshRenderer.mesh;
    json["material"] = meshRenderer.material;
    return json;
}

json GetJsonFromTextComponent(const TextComponent &text)
{
    json json;
    json["text"] = text.text;
    json["spacing"] = text.spacing;
    json["fontId"] = text.fontId;
    json["forgroundColor"] = {text.forgroundColor.r, text.forgroundColor.g, text.forgroundColor.b, text.forgroundColor.a};
    json["backgroundColor"] = {text.backgroundColor.r, text.backgroundColor.g, text.backgroundColor.b, text.backgroundColor.a};
    return json;
}

json GetJsonFromTransformComponent(const Transform &transform)
{
    json json;
    json["position"] = {transform.position.x, transform.position.y, transform.position.z};
    json["rotation"] = {transform.rotation.x, transform.rotation.y, transform.rotation.z};
    json["scale"] = {transform.scale.x, transform.scale.y, transform.scale.z};
    return json;
}

json GetJsonFromLightComponent(const Light &light)
{
    json json;
    json["color"] = {light.GetColor().r, light.GetColor().g, light.GetColor().b};
    json["position"] = {light.GetPosition().x, light.GetPosition().y, light.GetPosition().z};
    json["direction"] = {light.GetDirection().x, light.GetDirection().y, light.GetDirection().z};
    json["intensity"] = light.GetIntensity();
    json["type"] = light.GetType();

    return json;
}

json GetJsonFromEntity(const Entity &entity)
{
    json json;
    const EntityMetadata &data = entity.GetComponent<EntityMetadata>();

    json["entityMetadata"] = GetJsonFromEntityMetadata(data);

    if (entity.HasComponent<Transform>())
    {
        const Transform &transform = entity.GetComponent<Transform>();
        json["transform"] = GetJsonFromTransformComponent(transform);
    }
    if (entity.HasComponent<MeshRendererComponent>())
    {
        const MeshRendererComponent &meshRendererComponent = entity.GetComponent<MeshRendererComponent>();
        json["meshRendererComponent"] = GetJsonFromMeshRenderer(meshRendererComponent);
    }
    if (entity.HasComponent<TextComponent>())
    {
        const TextComponent &textComTextComponent = entity.GetComponent<TextComponent>();
        json["textComponent"] = GetJsonFromTextComponent(textComTextComponent);
    }
    if (entity.HasComponent<Light>())
    {
        const Light &light = entity.GetComponent<Light>();
        json["light"] = GetJsonFromLightComponent(light);
    }

    return json;
}

void WriteFontManager(json &json)
{
    json::array_t array;
    for (const auto &[id, font] : FontManager::GetMap())
    {
        array.emplace_back(font.GetName());
    }

    json["fonts"] = array;
}

void WriteMaterialManager(json &json)
{
    json::array_t array;
    for (const auto &[id, material] : MaterialManager::GetMap())
    {
        if (!material.enableSerializing)
        {
            continue;
        }
        array.emplace_back(GetJsonFromMaterial(material));
    }

    json["materials"] = array;
}

void WriteShaderManager(json &json)
{
    json::array_t array;

    for (const auto &[id, shader] : ShaderManager::GetMap())
    {
        if (!shader.vertexPath.empty())
        {
            array.emplace_back(GetJsonFromShader(shader));
        }
    }

    json["shaders"] = array;
}

void WriteTextureManager(json &json)
{
    json::array_t array;

    for (const auto &[id, texture] : TextureManager::GetMap())
    {
        if (!texture->IsEnableSerializing())
        {
            continue;
        }
        array.emplace_back(GetJsonFromTexture(*texture));
    }

    json["textures"] = array;
}

void WriteMeshManager(json &json, std::string_view vertexDataStorage, std::string_view indexDataStorage)
{
    FILE *vertexFile = fopen(vertexDataStorage.data(), "wb");
    FILE *indexFile = fopen(indexDataStorage.data(), "wb");

    json::array_t array;

    uint32_t relativeIndex = 0;

    for (const auto &[id, mesh] : MeshManager::GetMap())
    {
        if (!mesh->IsSerializingEnabled())
        {
            continue;
        }

        size_t vertexOffset = ftell(vertexFile);
        uint32_t vertexCount = mesh->GetVertexBuffer().capacity / sizeof(Vertex);
        size_t indexOffset = ftell(indexFile);
        uint32_t indexCount = mesh->GetIndexBuffer().capacity / sizeof(uint32_t);

        fwrite(mesh->GetVertexData(), vertexCount * sizeof(Vertex), 1, vertexFile);
        fwrite(mesh->GetIndexData(), indexCount * sizeof(uint32_t), 1, indexFile);

        array.emplace_back(GetJsonFromMesh(*mesh, vertexDataStorage, indexDataStorage, vertexOffset, vertexCount, indexOffset, indexCount));
    }

    json["meshes"] = array;
}

void WriteEntities(json &json, Scene &scene)
{
    json::array_t array;
    for (const auto &[entity, metadata] : scene.GetEntities<EntityMetadata>())
    {
        if (!metadata.enableSerializing)
        {
            continue;
        }

        array.emplace_back(GetJsonFromEntity(entity));
    }
    json["entities"] = array;
}

void SceneSerializer::Export(std::string_view filename, Scene &scene)
{
    json json;
    WriteFontManager(json);
    WriteShaderManager(json);
    WriteTextureManager(json);
    WriteMaterialManager(json);
    WriteMeshManager(json, "vertex.bin", "index.bin");
    WriteEntities(json, scene);

    std::ofstream out(filename.data());
    out << json.dump(4);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////                 Importer                  ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Maps
{
    std::unordered_map<uint32_t, FontID> fontIDMap;
    std::unordered_map<uint32_t, TextureID> textureIDMap;
    std::unordered_map<uint32_t, MaterialID> materialIDMap;
    std::unordered_map<uint32_t, MeshID> meshIDMap;
    std::unordered_map<uint32_t, ShaderID> shaderIDMap;
    std::unordered_map<uint32_t, EntityID> entityIDMap;
};

void LoadFonts(const json::array_t &array, Maps &maps)
{
    for (int i = 0; i < array.size(); i++)
    {
        maps.fontIDMap[i] = FontManager::Load(std::string(array[i]));
    }
}

void LoadTextures(const json::array_t &array, Maps &maps)
{
    for (int i = 0; i < array.size(); i++)
    {
        maps.textureIDMap[i] = TextureManager::LoadTexture(std::string(array[i]["filename"]));
        std::shared_ptr<Texture> texture = TextureManager::GetTexture(maps.textureIDMap[i]);
        texture->EnableSerializing(true);
    }
}

void LoadMaterials(const json::array_t &array, Maps &maps)
{
    for (int i = 0; i < array.size(); i++)
    {
        const json &json = array[i];
        Material material;

        material.albedoTexture = maps.textureIDMap[json["albedoTexture"]];
        material.metallicTexture = maps.textureIDMap[json["metallicTexture"]];
        material.roughnessTexture = maps.textureIDMap[json["roughnessTexture"]];
        material.normalTexture = maps.textureIDMap[json["normalTexture"]];
        material.shader = json["shader"];
        material.cullMode = json["cullMode"];
        material.roughnessFactor = json["roughnessFactor"];
        material.metallicFactor = json["metallicFactor"];
        material.indexOfRefraction = json["indexOfRefraction"];
        material.enableDepthWrite = json["enableDepthWrite"];
        material.enableDepthTest = json["enableDepthTest"];
        material.enableBlending = json["enableBlending"];
        material.name = json["name"];
        material.colorFactor.r = json["colorFactor"][0];
        material.colorFactor.g = json["colorFactor"][1];
        material.colorFactor.b = json["colorFactor"][2];
        material.colorFactor.a = json["colorFactor"][3];
        material.enableSerializing = true;

        maps.materialIDMap[i] = MaterialManager::AddMaterial(material);
    }
}

unsigned char *GetDataFromBinaryFile(std::string_view filename)
{
    FILE *fp = fopen(filename.data(), "rb");
    fseek(fp, 0L, SEEK_END);
    size_t fileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    unsigned char *data = new unsigned char[fileSize];

    fread(data, fileSize, 1, fp);
    fclose(fp);

    return data;
}

void LoadMeshes(const json::array_t &array, Maps &maps, std::string_view vertexFile, std::string_view indexFile)
{
    unsigned char *vertexData = GetDataFromBinaryFile(vertexFile);
    unsigned char *indexData = GetDataFromBinaryFile(indexFile);

    for (int i = 0; i < array.size(); i++)
    {
        const json &json = array[i];

        uint32_t indexCount = json["indexCount"];
        uint32_t vertexCount = json["vertexCount"];

        size_t indexOffset = json["indexOffset"];
        size_t vertexOffset = json["vertexOffset"];

        std::string name = json["name"];

        std::vector<Vertex> vertices;
        vertices.resize(vertexCount);
        memcpy(vertices.data(), vertexData + vertexOffset, vertexCount * sizeof(Vertex));

        std::vector<uint32_t> indices;
        indices.resize(indexCount);
        memcpy(indices.data(), indexData + indexOffset, indexCount * sizeof(uint32_t));

        MeshID id = MeshManager::CreateMesh(vertices, indices);
        maps.meshIDMap[i] = id;
        std::shared_ptr<Mesh> mesh = MeshManager::GetMesh(id);
        mesh->SetName(name);
        mesh->EnableSerializing(true);
    }
}

void LoadEntities(Scene &scene, const json::array_t &array, Maps &maps)
{
    for (int i = 0; i < array.size(); i++)
    {
        EntityMetadata metadata;
        const json &metadataJson = array[i]["entityMetadata"];

        metadata.name = metadataJson["name"];
        metadata.createdFromModel = true;
        metadata.enableSerializing = true;

        Entity entity = scene.CreateEntity(metadata.name);
        entity.GetComponent<EntityMetadata>() = metadata;

        if (array[i].contains("meshRendererComponent"))
        {
            const json &meshRendererJson = array[i]["meshRendererComponent"];

            MeshRendererComponent &meshRenderer = entity.AddComponent<MeshRendererComponent>();

            uint32_t meshIndex = meshRendererJson["mesh"];
            uint32_t materialIndex = meshRendererJson["material"];

            meshRenderer.mesh = maps.meshIDMap.contains(meshIndex) ? maps.meshIDMap[meshIndex] : INVALID_MESH_ID;
            meshRenderer.material = maps.materialIDMap.contains(materialIndex) ? maps.materialIDMap[materialIndex] : INVALID_MATERIAL_ID;
        }
        if (array[i].contains("transform"))
        {
            const json &transformJson = array[i]["transform"];

            Transform &transform = entity.AddComponent<Transform>();
            transform.position = {transformJson["position"][0], transformJson["position"][1], transformJson["position"][2]};
            transform.rotation = {transformJson["rotation"][0], transformJson["rotation"][1], transformJson["rotation"][2]};
            transform.scale = {transformJson["scale"][0], transformJson["scale"][1], transformJson["scale"][2]};
        }
        if (array[i].contains("textComponent"))
        {
            const json &textComponentJson = array[i]["textComponent"];

            TextComponent &textComponent = entity.AddComponent<TextComponent>();
            textComponent.text = textComponentJson["text"];
            textComponent.spacing = textComponentJson["spacing"];
            textComponent.fontId = maps.fontIDMap[textComponentJson["fontId"]];
            textComponent.forgroundColor = {textComponentJson["forgroundColor"][0], textComponentJson["forgroundColor"][1], textComponentJson["forgroundColor"][2], textComponentJson["forgroundColor"][3]};
            textComponent.backgroundColor = {textComponentJson["backgroundColor"][0], textComponentJson["backgroundColor"][1], textComponentJson["backgroundColor"][2], textComponentJson["backgroundColor"][3]};
        }
        if (array[i].contains("light"))
        {
            const json &lightJson = array[i]["light"];
            Light &light = entity.AddComponent<Light>();
            light.SetColor({lightJson["color"][0], lightJson["color"][1], lightJson["color"][2]});
            light.SetPosition({lightJson["position"][0], lightJson["position"][1], lightJson["position"][2]});
            light.SetDirection({lightJson["direction"][0], lightJson["direction"][1], lightJson["direction"][2]});
            light.SetIntensity(lightJson["intensity"]);
            light.SetType(lightJson["type"]);
        }
    }
}

void SceneSerializer::Import(std::string_view filename, Scene &scene)
{
    Maps maps;

    std::ifstream input(filename.data());
    json json = json::parse(input);

    LoadFonts(json["fonts"], maps);
    LoadTextures(json["textures"], maps);
    LoadMaterials(json["materials"], maps);
    LoadMeshes(json["meshes"], maps, "vertex.bin", "index.bin");
    LoadEntities(scene, json["entities"], maps);
}