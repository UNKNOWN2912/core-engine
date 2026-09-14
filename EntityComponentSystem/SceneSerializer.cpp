#include "SceneSerializer.hpp"
#include "Core/Application.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/Transform.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <string.h>

using namespace nlohmann;

struct DataPartition
{
    size_t offset = 0;
    size_t size = 0;
};

json::array_t SerializeFontManager(const FontManager &fontManager)
{
    json::array_t fontArray;
    for (const auto &[id, font] : fontManager.GetMap())
    {
        json fontJson;
        fontJson["filename"] = font.GetFileName();
        fontJson["id"] = id;
        fontArray.push_back(fontJson);
    }

    return fontArray;
}

json::array_t SerializeShaderManager(const ShaderManager &shaderManager)
{
    json::array_t shaderArray;
    for (const auto &[id, shader] : shaderManager.GetMap())
    {
        json shaderJson;
        shaderJson["vertexPath"] = shader.GetVertexFilename();
        shaderJson["fragmentPath"] = shader.GetFragmentFilename();
        shaderJson["geometryPath"] = shader.GetGeometryFilename();
        shaderJson["tessellationPath"] = shader.GetTessellationFilename();
        shaderJson["id"] = id;
        shaderJson["enableDepthWrite"] = shader.GetSettings().enableDepthWrite;
        shaderJson["enableDepthTest"] = shader.GetSettings().enableDepthTest;
        shaderJson["primitive"] = shader.GetSettings().primitive;
        shaderJson["cullMode"] = shader.GetSettings().cullMode;
        shaderJson["sampleCount"] = shader.GetSettings().sampleCount;

        shaderArray.push_back(shaderJson);
    }

    return shaderArray;
}

json::array_t SerializeTextureManager(const TextureManager &textureManager)
{
    json::array_t textureArray;
    for (const auto &[id, texture] : textureManager.GetMap())
    {
        json textureJson;
        textureJson["filename"] = texture.GetFilename();
        textureJson["id"] = id;

        textureArray.push_back(textureJson);
    }

    return textureArray;
}

json::array_t SerializeMaterialManager(const MaterialManager &materialManager)
{
    json::array_t materialArray;
    for (const auto &[id, material] : materialManager.GetMap())
    {
        json materialJson;

        materialJson["shader"] = material.shader;
        materialJson["albedoTexture"] = material.albedoTexture;
        materialJson["roughnessTexture"] = material.roughnessTexture;
        materialJson["metallicTexture"] = material.metallicTexture;
        materialJson["normalTexture"] = material.normalTexture;
        materialJson["cullMode"] = material.cullMode;
        materialJson["colorFactor"] = {material.colorFactor.r, material.colorFactor.g, material.colorFactor.b, material.colorFactor.a};
        materialJson["roughnessFactor"] = material.roughnessFactor;
        materialJson["metallicFactor"] = material.metallicFactor;
        materialJson["indexOfRefraction"] = material.indexOfRefraction;
        materialJson["enableDepthWrite"] = material.enableDepthWrite;
        materialJson["enableDepthTest"] = material.enableDepthTest;
        materialJson["enableBlending"] = material.enableBlending;
        materialJson["name"] = material.name;
        materialJson["id"] = id;
        materialJson["drawPriority"] = material.drawPriority;

        materialArray.push_back(materialJson);
    }

    return materialArray;
}

json::array_t SerializeMeshManager(const MeshManager &meshManager)
{
    json::array_t meshArray;

    FILE *vertexFile = fopen("vertex.bin", "wb");
    FILE *indexFile = fopen("index.bin", "wb");

    size_t vertexOffset = 0;
    size_t indexOffset = 0;

    for (const auto &[id, mesh] : meshManager.GetMap())
    {
        fwrite(mesh.GetVertexData(), mesh.GetVertexBuffer().capacity, 1, vertexFile);
        fwrite(mesh.GetIndexData(), mesh.GetIndexBuffer().capacity, 1, indexFile);

        json meshJson;
        meshJson["vertexSize"] = mesh.GetVertexSize();
        meshJson["vertexOffset"] = vertexOffset;
        meshJson["indexSize"] = mesh.GetIndexSize();
        meshJson["indexOffset"] = indexOffset;
        meshJson["id"] = id;

        vertexOffset += mesh.GetVertexSize();
        indexOffset += mesh.GetIndexSize();

        meshArray.push_back(meshJson);
    }
    fclose(vertexFile);
    fclose(indexFile);

    return meshArray;
}

json SerializeTransform(const Transform &transform)
{
    json transformJson;
    transformJson["position"] = {transform.position.x, transform.position.y, transform.position.z};
    transformJson["rotation"] = {transform.rotation.x, transform.rotation.y, transform.rotation.z};
    transformJson["scale"] = {transform.scale.x, transform.scale.y, transform.scale.z};
    return transformJson;
}

json SerializeMeshRenderer(const MeshRendererComponent &meshRenderer)
{
    json meshRendererJson;
    meshRendererJson["mesh"] = meshRenderer.mesh;
    meshRendererJson["material"] = meshRenderer.material;
    return meshRendererJson;
}

json SerializeLight(const Light &light)
{
    json lightJson;
    lightJson["position"] = {light.GetPosition().x, light.GetPosition().y, light.GetPosition().z};
    lightJson["direction"] = {light.GetDirection().x, light.GetDirection().y, light.GetDirection().z};
    lightJson["color"] = {light.GetColor().r, light.GetColor().g, light.GetColor().b};
    lightJson["outerCosinAngle"] = light.GetOuterCosinAngle();
    lightJson["innerCosinAngle"] = light.GetInnerCosinAngle();
    lightJson["shadowMapResolution"] = light.GetShadowMapResolution();
    lightJson["intensity"] = light.GetIntensity();
    lightJson["type"] = light.GetType();

    return lightJson;
}

json SerializeText(const TextComponent &textComponent)
{
    json textJson;
    textJson["text"] = textComponent.text;
    textJson["spacing"] = textComponent.spacing;
    textJson["font"] = textComponent.font;
    textJson["forgroundColor"] = {textComponent.forgroundColor.r, textComponent.forgroundColor.g, textComponent.forgroundColor.b, textComponent.forgroundColor.a};
    textJson["backgroundColor"] = {textComponent.backgroundColor.r, textComponent.backgroundColor.g, textComponent.backgroundColor.b, textComponent.backgroundColor.a};

    return textJson;
}

json SerializeMetadata(const EntityMetadata &metadata)
{
    json metadataJson;
    metadataJson["name"] = metadata.name;

    return metadataJson;
}

json::array_t SerializeEntities(const Scene &scene)
{
    json::array_t entityArray;

    scene.Each<EntityMetadata>([&](const Entity entity, const EntityMetadata &metadata) {
        json entityJson;
        if (entity.HasComponent<Transform>())
        {
            entityJson["transform"] = SerializeTransform(entity.GetComponent<Transform>());
        }
        if (entity.HasComponent<EntityMetadata>())
        {
            entityJson["metadata"] = SerializeMetadata(entity.GetComponent<EntityMetadata>());
        }
        if (entity.HasComponent<MeshRendererComponent>())
        {
            entityJson["meshRenderer"] = SerializeMeshRenderer(entity.GetComponent<MeshRendererComponent>());
        }
        if (entity.HasComponent<Light>())
        {
            entityJson["light"] = SerializeLight(entity.GetComponent<Light>());
        }
        if (entity.HasComponent<TextComponent>())
        {
            entityJson["text"] = SerializeText(entity.GetComponent<TextComponent>());
        }

        entityArray.push_back(entityJson);
    });

    return entityArray;
}

void SceneSerializer::Export(std::string_view filename, const Scene &scene)
{
    json json;
    json["fonts"] = SerializeFontManager(scene.GetResourceManager().GetFontManager());
    json["shaders"] = SerializeShaderManager(scene.GetResourceManager().GetShaderManager());
    json["textures"] = SerializeTextureManager(scene.GetResourceManager().GetTextureManager());
    json["materials"] = SerializeMaterialManager(scene.GetResourceManager().GetMaterialManager());
    json["meshes"] = SerializeMeshManager(scene.GetResourceManager().GetMeshManager());
    json["entities"] = SerializeEntities(scene);

    std::ofstream output(filename.data());
    output << json;
}

void DeserializeFontManager(const json::array_t &fontJsonArray, FontManager &fontManager)
{
    for (const json &json : fontJsonArray)
    {
        std::string filename = json["filename"];
        std::string id = json["id"];
        fontManager.Load(filename, id);
    }
}

void DeserializeShaderManager(const json::array_t &shaderJsonArray, ShaderManager &shaderManager)
{
    const Renderer &renderer = Application::GetInstance()->GetRenderer();
    for (const json &json : shaderJsonArray)
    {
        std::string vertexPath = json["vertexPath"];
        std::string fragmentPath = json["fragmentPath"];
        std::string geometryPath = json["geometryPath"];
        std::string tessellationPath = json["tessellationPath"];
        std::string id = json["id"];

        ShaderConfig config = renderer.GetSceneShaderConfig();
        config.cullMode = json["cullMode"];
        config.enableDepthTest = json["enableDepthTest"];
        config.enableDepthWrite = json["enableDepthWrite"];
        config.primitive = json["primitive"];
        config.sampleCount = json["sampleCount"];
        shaderManager.Load(id, vertexPath, fragmentPath, geometryPath, tessellationPath, config);
    }
}

void DeserializeTextureManager(const json::array_t &textureJsonArray, TextureManager &textureManager)
{
    for (const json &json : textureJsonArray)
    {
        std::string filename = json["filename"];
        std::string id = json["id"];
        textureManager.LoadTexture(id, filename);
    }
}

void DeserializeMaterialManager(const json::array_t &materialJsonArray, MaterialManager &materialManager)
{
    for (const json &json : materialJsonArray)
    {
        Material material;
        material.shader = json["shader"];
        material.albedoTexture = json["albedoTexture"];
        material.roughnessTexture = json["roughnessTexture"];
        material.metallicTexture = json["metallicTexture"];
        material.normalTexture = json["normalTexture"];
        material.cullMode = json["cullMode"];
        material.colorFactor = {json["colorFactor"][0], json["colorFactor"][1], json["colorFactor"][2], json["colorFactor"][3]};
        material.roughnessFactor = json["roughnessFactor"];
        material.metallicFactor = json["metallicFactor"];
        material.indexOfRefraction = json["indexOfRefraction"];
        material.enableDepthWrite = json["enableDepthWrite"];
        material.enableDepthTest = json["enableDepthTest"];
        material.enableBlending = json["enableBlending"];
        material.name = json["name"];
        material.drawPriority = json["drawPriority"];

        std::string id = json["id"];
        materialManager.AddMaterial(material, id);
    }
}

void DeserializeMeshManager(const json::array_t &meshJsonArray, MeshManager &meshManager)
{
    Vertex *vertexData = nullptr;
    FILE *vertexFile = fopen("vertex.bin", "rb");

    fseek(vertexFile, 0L, SEEK_END);
    size_t vertexTotalSize = ftell(vertexFile);
    fseek(vertexFile, 0L, SEEK_SET);

    vertexData = new Vertex[vertexTotalSize / sizeof(Vertex)];
    fread(vertexData, vertexTotalSize, 1, vertexFile);
    fclose(vertexFile);

    uint32_t *indexData = nullptr;
    FILE *indexFile = fopen("index.bin", "rb");

    fseek(indexFile, 0L, SEEK_END);
    size_t indexTotalSize = ftell(indexFile);
    fseek(indexFile, 0L, SEEK_SET);

    indexData = new uint32_t[indexTotalSize / sizeof(uint32_t)];
    fread(indexData, indexTotalSize, 1, indexFile);
    fclose(indexFile);

    for (const json &json : meshJsonArray)
    {
        size_t vertexSize = json["vertexSize"];
        size_t vertexOffset = json["vertexOffset"];
        size_t indexSize = json["indexSize"];
        size_t indexOffset = json["indexOffset"];

        assert(vertexSize <= vertexTotalSize);
        assert(vertexOffset + vertexSize <= vertexTotalSize);
        assert(indexSize <= indexTotalSize);
        assert(indexOffset + indexSize <= indexTotalSize);

        vertexSize /= sizeof(Vertex);
        vertexOffset /= sizeof(Vertex);
        indexSize /= sizeof(uint32_t);
        indexOffset /= sizeof(uint32_t);

        std::vector<Vertex> vertices;
        vertices.resize(vertexSize);
        memcpy(vertices.data(), &vertexData[vertexOffset], vertexSize * sizeof(Vertex));

        std::vector<uint32_t> indices;
        indices.resize(indexSize);
        memcpy(indices.data(), &indexData[indexOffset], indexSize * sizeof(uint32_t));

        std::string id = json["id"];
        meshManager.CreateMesh(vertices, indices, id);
    }

    delete vertexData;
    delete indexData;
}

Transform DeserializeTransform(const json &json)
{
    Transform transform;
    transform.position = {json["position"][0], json["position"][1], json["position"][2]};
    transform.rotation = {json["rotation"][0], json["rotation"][1], json["rotation"][2]};
    transform.scale = {json["scale"][0], json["scale"][1], json["scale"][2]};
    return transform;
}

MeshRendererComponent DeserializeMeshRenderer(const json &json)
{
    MeshRendererComponent meshRenderer;
    meshRenderer.mesh = json["mesh"];
    meshRenderer.material = json["material"];

    return meshRenderer;
}

Light DeserializeLight(const json &json)
{
    Light light;
    light.SetPosition({json["position"][0], json["position"][1], json["position"][2]});
    light.SetDirection({json["direction"][0], json["direction"][1], json["direction"][2]});
    light.SetColor({json["color"][0], json["color"][1], json["color"][2]});
    light.SetOuterCosinAngle(json["outerCosinAngle"]);
    light.SetInnerCosinAngle(json["innerCosinAngle"]);
    light.SetShadowMapResolution(json["shadowMapResolution"]);
    light.SetIntensity(json["intensity"]);
    light.SetType(json["type"]);
    return light;
}

TextComponent DeserializeText(const json &json)
{
    TextComponent textComponent;
    textComponent.text = json["text"];
    textComponent.spacing = json["spacing"];
    textComponent.font = json["font"];
    textComponent.forgroundColor = {json["forgroundColor"][0], json["forgroundColor"][1], json["forgroundColor"][2], json["forgroundColor"][3]};      //= {textComponent.forgroundColor.r, textComponent.forgroundColor.g, textComponent.forgroundColor.b, textComponent.forgroundColor.a};
    textComponent.backgroundColor = {json["backgroundColor"][0], json["backgroundColor"][1], json["backgroundColor"][2], json["backgroundColor"][3]}; //= {textComponent.backgroundColor.r, textComponent.backgroundColor.g, textComponent.backgroundColor.b, textComponent.backgroundColor.a};

    return textComponent;
}

EntityMetadata DeserializeMetadata(const json &json)
{
    EntityMetadata metadata;
    metadata.name = json["name"];

    return metadata;
}

void DeserializeEntities(const json::array_t &entityJsonArray, Scene &scene)
{
    for (const json &json : entityJsonArray)
    {
        Entity entity = scene.CreateEntity("");
        if (json.contains("transform"))
        {
            Transform transform = DeserializeTransform(json["transform"]);
            entity.AddComponent<Transform>(transform);
        }
        if (json.contains("meshRenderer"))
        {
            MeshRendererComponent meshRenderer = DeserializeMeshRenderer(json["meshRenderer"]);
            entity.AddComponent<MeshRendererComponent>(meshRenderer);
        }
        if (json.contains("text"))
        {
            TextComponent textComponent = DeserializeText(json["text"]);
            entity.AddComponent<TextComponent>(textComponent);
        }
        if (json.contains("metadata"))
        {
            EntityMetadata metadata = DeserializeMetadata(json["metadata"]);
            entity.GetComponent<EntityMetadata>() = metadata;
        }
        if (json.contains("light"))
        {
            Light light = DeserializeLight(json["light"]);
            entity.AddComponent<Light>(std::move(light));
        }
    }
}

void SceneSerializer::Import(std::string_view filename, Scene &scene)
{
    std::ifstream input(filename.data());
    json json = json::parse(input);

    if (json.contains("fonts"))
    {
        DeserializeFontManager(json["fonts"], scene.GetResourceManager().GetFontManager());
    }
    if (json.contains("meshes"))
    {
        DeserializeMeshManager(json["meshes"], scene.GetResourceManager().GetMeshManager());
    }
    if (json.contains("materials"))
    {
        DeserializeMaterialManager(json["materials"], scene.GetResourceManager().GetMaterialManager());
    }
    if (json.contains("textures"))
    {
        DeserializeTextureManager(json["textures"], scene.GetResourceManager().GetTextureManager());
    }
    if (json.contains("shaders"))
    {
        DeserializeShaderManager(json["shaders"], scene.GetResourceManager().GetShaderManager());
    }

    if (json.contains("entities"))
    {
        DeserializeEntities(json["entities"], scene);
    }
}
