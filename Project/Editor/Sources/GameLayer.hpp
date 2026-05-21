#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "EntityComponentSystem/EntityComponentSystem.hpp"
#include "Renderer/Camera.hpp"

struct UniformData
{
    glm::vec3 lightPosition = glm::vec3(0);
    float intensity = 0;
};

class GameLayer : public Layer
{
  public:
    void OnAttach() override;
    void OnUpdate() override;
    void OnDetach() override;

    void UpdateCamera();

    void DestroyMaterial();

    void CreateMaterial();

    void ReloadMaterial();

    Camera mCamera;
    CameraController mCameraController;

    RenderTarget mTarget;
    Material skyboxMaterial;
    StaticMesh mesh;

    Scene scene;
};