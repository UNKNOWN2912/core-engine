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
    ShaderID shader = INVALID_SHADER_ID;

    TextureID albedoTexture = INVALID_TEXTURE_ID;
    TextureID roughnessTexture = INVALID_TEXTURE_ID;
    TextureID metallicTexture = INVALID_TEXTURE_ID;
    TextureID normalTexture = INVALID_TEXTURE_ID;

    CullMode cullMode = CullMode::Back;

    glm::vec4 colorFactor = glm::vec4(0);
    float roughnessFactor = 0.5;
    float metallicFactor = 0.0;

    float indexOfRefraction{};

    bool enableDepthWrite = true;
    bool enableDepthTest = true;
    bool enableBlending = true;

    bool enableSerializing = false;

    std::string name = "Untitled";
};