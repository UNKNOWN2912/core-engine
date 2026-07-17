#include "SceneSerializer.hpp"
#include "Assets/ModelImporter.hpp"
#include "Core/Macro.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "Renderer/Transform.hpp"
#include "json.hpp"
#include <format>
#include <fstream>

using namespace nlohmann;

void SceneSerializer::Export(std::string_view filename, Scene &scene)
{
    auto &entities = scene.GetEntities<EntityMetadata>();

    json json;

    auto &dependencyJson = json["ModelDependency"];
    dependencyJson["count"] = scene.GetModelFileImporter().size();
    for (int i = 0; i < scene.GetModelFileImporter().size(); i++)
    {
        std::string indexString = std::format("{}", i);

        dependencyJson[std::to_string(i)] = scene.GetModelFileImporter()[i];
    }

    auto &entityJson = json["Entities"];
    entityJson["count"] = entities.size();
    for (const auto &[entity, metadata] : entities)
    {
        std::string idString = std::format("{}", (uint32_t)entity.GetId());

        {
            auto &jsonMetadata = entityJson[std::to_string((uint32_t)entity.GetId())]["EntityMetadata"];
            {
                jsonMetadata["name"] = metadata.name;
                jsonMetadata["createdFromModel"] = metadata.createdFromModel;
            }
        }

        if (entity.HasComponent<Transform>())
        {
            const Transform &transform = entity.GetComponent<Transform>();
            auto &jsonTransform = entityJson[idString]["Transform"];
            jsonTransform["position"] = {transform.position.x, transform.position.y, transform.position.z};
            jsonTransform["rotation"] = {transform.rotation.x, transform.rotation.y, transform.rotation.z};
            jsonTransform["scale"] = {transform.scale.x, transform.scale.y, transform.scale.z};
        }

        if (entity.HasComponent<MeshRenderer>())
        {
            const MeshRenderer &meshRenderer = entity.GetComponent<MeshRenderer>();
            auto &jsonMeshRenderer = entityJson[idString]["MeshRenderer"];
            jsonMeshRenderer["mesh"] = meshRenderer.mesh;
            jsonMeshRenderer["material"] = meshRenderer.material;
        }

        if (entity.HasComponent<Light>())
        {
            const Light &light = entity.GetComponent<Light>();
            auto &jsonLight = entityJson[idString]["Light"];

            const std::string_view lightTypeString[] = {
                "DirectionalLight",
                "PointLight",
                "SpotLight",
            };

            jsonLight["color"] = {light.GetColor().x, light.GetColor().y, light.GetColor().z};
            jsonLight["intensity"] = light.GetIntensity();
            jsonLight["type"] = light.GetType();
        }
    }
    auto &materialJson = json["Materials"];

    const auto &materialMap = MaterialManager::GetMap();
    materialJson["count"] = materialMap.size();
    for (const auto &[id, material] : materialMap)
    {
        std::string idString = std::to_string((uint32_t)id);

        materialJson[idString]["name"] = material->name;
        materialJson[idString]["colorFactor"] = {material->colorFactor.x, material->colorFactor.y, material->colorFactor.z};
        materialJson[idString]["roughnessFactor"] = material->roughnessFactor;
        materialJson[idString]["metallicFactor"] = material->metallicFactor;
        materialJson[idString]["enableDepthWrite"] = material->enableDepthWrite;
        materialJson[idString]["enableDepthTest"] = material->enableDepthTest;
        materialJson[idString]["enableBlending"] = material->enableBlending;
        materialJson[idString]["cullMode"] = material->cullMode;

        for (int i = 0; i < 32; i++)
        {
            materialJson[idString]["textures"][i] = material->textures[i];
        }
    }

    std::ofstream output(filename.data());
    output << json;
}

void SceneSerializer::Import(std::string_view filename, Scene &scene)
{
    std::ifstream file(filename.data());
    json json = json::parse(file);

    if (json.contains("ModelDependency"))
    {
        ModelImporter importer;
        const auto &modelDependencyJson = json["ModelDependency"];
        uint32_t count = modelDependencyJson["count"];
        for (int i = 0; i < count; i++)
        {
            std::string filename = (std::string)modelDependencyJson[std::to_string(i)];
            importer.Import(filename, scene);
        }
    }

    if (json.contains("Entities"))
    {
        const auto &entitiesJson = json["Entities"];
        uint32_t count = entitiesJson["count"];

        for (int i = 0; i < count; i++)
        {
            const auto &entityJson = entitiesJson[std::to_string(i)];
            Entity entity;
            if (entityJson["EntityMetadata"]["createdFromModel"] == true)
            {
                entity = scene.GetEntityByName((std::string)entityJson["EntityMetadata"]["name"]);
            }
            else
            {
                entity = scene.CreateEntity((std::string)entityJson["EntityMetadata"]["name"]);
            }

            if (entityJson.contains("Transform"))
            {
                glm::vec3 position = {0, 0, 0};
                position.x = entityJson["Transform"]["position"][0];
                position.y = entityJson["Transform"]["position"][1];
                position.z = entityJson["Transform"]["position"][2];

                glm::vec3 rotation = {0, 0, 0};
                rotation.x = entityJson["Transform"]["rotation"][0];
                rotation.y = entityJson["Transform"]["rotation"][1];
                rotation.z = entityJson["Transform"]["rotation"][2];

                glm::vec3 scale = {0, 0, 0};
                scale.x = entityJson["Transform"]["scale"][0];
                scale.y = entityJson["Transform"]["scale"][1];
                scale.z = entityJson["Transform"]["scale"][2];

                entity.AddComponent<Transform>(position, rotation, scale);
            }

            if (entityJson.contains("MeshRenderer"))
            {
                entity.AddComponent<MeshRenderer>(entityJson["MeshRenderer"]["mesh"], entityJson["MeshRenderer"]["material"]);
            }

            if (entityJson.contains("Light"))
            {
                const std::string_view lightTypeString[] = {
                    "DirectionalLight",
                    "PointLight",
                    "SpotLight",
                };

                glm::vec3 color = {0, 0, 0};
                color.x = entityJson["Light"]["color"][0];
                color.y = entityJson["Light"]["color"][1];
                color.z = entityJson["Light"]["color"][2];

                float intensity = entityJson["Light"]["intensity"];

                LightType type = entitiesJson["Light"]["type"];

                Light &light = entity.AddComponent<Light>();
                light.SetColor(color);
                light.SetIntensity(intensity);
                light.SetType(type);
            }
        }
    }
}
