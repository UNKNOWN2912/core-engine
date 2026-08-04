#pragma once
#include "Assets/FontManager.hpp"
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Renderer/Light.hpp"

struct MeshRendererComponent
{
    std::string mesh;
    std::string material;

    MeshRendererComponent() = default;
    MeshRendererComponent(std::string_view meshId, std::string_view materialId)
        : mesh(meshId), material(materialId)
    {
    }
};

struct TextComponent
{
    std::string text;
    float spacing = 1.f;
    std::string font;
    glm::vec4 forgroundColor = glm::vec4(1);
    glm::vec4 backgroundColor = glm::vec4(0);
};

struct CameraComponent
{
    Camera camera;
    bool action = false;
};
