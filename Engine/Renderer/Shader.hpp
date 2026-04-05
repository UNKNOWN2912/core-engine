#pragma once
#include <cstdint>

enum class ShaderType
{
    Vertex,
    Fragment,
    Compute
};

class Shader
{
    public:
        uint64_t GetId() const { return mId; }
        ShaderType GetType() const { return mType; }   

        Shader& SetId(uint64_t id) { mId = id; return *this; }
        Shader& SetType(ShaderType type) { mType = type; return *this; }


    private:
        ShaderType mType;
        uint64_t mId = 0;
};

