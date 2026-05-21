#pragma once
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Renderer/Transform.hpp"

struct MeshRendererComponent
{
    MeshID mesh;
    MaterialID material;
};