#pragma once
#include <cstdint>
#include <vector>

enum class VertexFormatType
{
    Float, Vec2, Vec3, Vec4,
    Int, IVec2, IVec3, IVec4,
    UInt, UVec2, UVec3, UVec4,
    Double, DVec2, DVec3, DVec4,
    Mat2, Mat3, Mat4,
    None
};

const size_t formatTypeSize[] = {
    4, 8, 12, 16,
    4, 8, 12, 16,
    4, 8, 12, 16,
    8, 16, 24, 32,
    16, 36, 64,
    0
};

struct Attribute
{
    uint32_t binding;
    uint32_t location;
    VertexFormatType format;
    uint32_t offset;
};

struct Binding
{
    uint32_t binding;
    uint32_t stride;
    bool perInstance;
};

class VertexLayout
{
    public:
        VertexLayout& AddAttribute(uint32_t binding, uint32_t location, uint32_t offset, VertexFormatType format) 
        {

            Attribute attribute;
            attribute.binding = binding;
            attribute.location = location;
            attribute.format = format;
            attribute.offset = offset;


            return *this;
        }
        VertexLayout& AddBinding(uint32_t binding, uint32_t stride, bool perInstance) 
        {
            Binding bind;
            bind.binding = binding;
            bind.stride = stride;
            bind.perInstance = perInstance;

            return *this;
        }

        const std::vector<Attribute>& GetAttributes() const { return mAttributes; }
        const std::vector<Binding>& GetBindings() const { return mBindings; }

    private:
        std::vector<Attribute> mAttributes;
        std::vector<Binding> mBindings;
};