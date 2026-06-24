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
    VertexShaderID vertexShader = ShaderManager::GetInvalidVertexShaderID();
    FragmentShaderID fragmentShader = ShaderManager::GetInvalidFragmentShaderID();
    GeometryShaderID geometryShader = ShaderManager::GetInvalidGeometryShaderID();

    union {
        TextureID textures[32] = {(TextureID)UINT64_MAX};
        struct
        {
            TextureID albedo;
            TextureID specular;
        };
    };

    CullMode cullMode = CullMode::Back;

    bool enableDepthWrite = true;
    bool enableDepthTest = true;
    bool enableBlending = true;

    Filter magFilter = Filter::Linear;
    Filter minFilter = Filter::Linear;
    AddressMode addressMode = AddressMode::Repeat;

    std::string name = "Untitled";
};