#pragma once
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Texture.hpp"

enum class CullMode
{
    None = 0,
    Front,
    Back,
};

enum class FrontFace
{
    None = 0,
    Clockwise, 
    CounterClockwise,
};

enum class PrimitiveType
{
    None = 0,
    Triangle, 
    Line, 
    Point,
};

struct MaterialSettings
{
    bool depthEnable = false;
    CullMode cullMode = CullMode::Back;
    PrimitiveType primitiveType = PrimitiveType::Triangle;
    FrontFace frontFace = FrontFace::Clockwise;
    float lineWidth = 1.f;
    int sampleCount = 1; 
};

class Material
{
    public:
        void LoadAlbedo(std::string_view filename);
        void LoadShaders(std::string_view vertexShader, std::string_view fragmentShader);
        

        void Create();
        
    private:
        Texture mAlbedo;
        GraphicsPipeline mPipeline;
        MaterialSettings mSettings;
};