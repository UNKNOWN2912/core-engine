#pragma once
#include "Assets/FontManager.hpp"
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Renderer/Light.hpp"

struct MeshRendererComponent
{
    MeshID mesh = INVALID_MESH_ID;
    MaterialID material = INVALID_MATERIAL_ID;

    MeshRendererComponent() = default;
    MeshRendererComponent(MeshID meshId, MaterialID materialId)
        : mesh(meshId), material(materialId)
    {
    }
};

struct TextComponent
{
    std::string text;
    float spacing = 1.f;
    FontID fontId = INVALID_FONT_ID;
    glm::vec4 color = glm::vec4(1);
};