#pragma once
#include "Assets/ShaderManager.hpp"
#include "Assets/TextureManager.hpp"
#include "Renderer/Types.hpp"

enum class AttributeType
{
    Int,
    UInt,
    Float,
    IVec2,
    UVec2,
    Vec2,
    IVec3,
    UVec3,
    Vec3,
    IVec4,
    UVec4,
    Vec4,
};

struct Material
{
    ShaderID shader;

    union {
        TextureID textures[32] = {(TextureID)UINT64_MAX};
        struct
        {
            TextureID albedo;
            TextureID roughness;
            TextureID metallic;
            TextureID normal;
        };
    };

    CullMode cullMode = CullMode::Back;

    glm::vec3 colorFactor = glm::vec3(0);
    float roughnessFactor = 0.5;
    float metallicFactor = 0.0;

    float indexOfRefraction;

    bool enableDepthWrite = true;
    bool enableDepthTest = true;
    bool enableBlending = true;

    std::string name = "Untitled";
};