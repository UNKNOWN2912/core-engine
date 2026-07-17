#pragma once
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Renderer/Light.hpp"

struct MeshRenderer
{
    MeshID mesh = (MeshID)UINT64_MAX;
    MaterialID material = (MaterialID)UINT64_MAX;

    MeshRenderer() = default;
    MeshRenderer(MeshID meshId, MaterialID materialId)
        : mesh(meshId), material(materialId)
    {
    }
};
