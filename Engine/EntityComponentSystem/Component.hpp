#pragma once
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"

struct MeshRendererComponent
{
    MeshID mesh = (MeshID)UINT64_MAX;
    MaterialID material = (MaterialID)UINT64_MAX;
};