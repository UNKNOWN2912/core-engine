#include <Engine.hpp>

enum class FaceType
{
    None = 0,
    Front,
    Back,
    Left,
    Right, 
    Top, 
    Bottom
};

class CubeFaceGenerator
{
    public:
        StaticMesh GenerateFace(FaceType type);
};

class CoreCraft : public Application
{
    void OnStart() override
    {

    }

    void OnUpdate() override
    {
        GetRendererRef().BeginFrame();


        GetRendererRef().EndFrame();
    }

    void OnEnd() override
    {

    }
};

Application* Application::Create()
{
    return new CoreCraft;
}