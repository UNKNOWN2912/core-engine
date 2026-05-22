#pragma once
#include "CameraController.hpp"
#include "EntityComponentSystem/EntityComponentSystem.hpp"
#include "Panel.hpp"
#include "imgui.h"
#include <Engine.hpp>

class GameViewPanel : public Panel
{
  public:
    GameViewPanel(RenderTarget *renderTarget, Camera *camera, CameraController *controller);

    void OnAttach() override;
    void OnUpdate() override;
    void OnRenderUi() override;

  private:
    bool mHideCursor = false;
    RenderTarget *mTarget;
    Camera *mCamera;
    CameraController *mController;
    ImTextureID mTexture;
    glm::uvec2 mSize = {1, 1};
};

class PerformancePanel : public Panel
{
  public:
    PerformancePanel();
    void OnRenderUi() override;
};

class ImageViewerPanel : public Panel
{
  public:
    ImageViewerPanel();
    void OnAttach() override;
    void OnUpdate() override;
    void OnRenderUi() override;
    void AddImage(std::string identifier, Image *image);

  private:
    std::string mSelection;
    std::unordered_map<std::string, Image *> mImageMap;
    ImTextureID mTexture = 0;
};

class EntityPanel : public Panel
{
  public:
    EntityPanel();
    Entity GetSelectedEntity()
    {
        return mSelectedEntity;
    }
    void SetScene(Scene *scene)
    {
        mScene = scene;
    }

    void OnRenderUi() override;

  private:
    Entity mSelectedEntity;
    std::string mSearch;
    Scene *mScene = nullptr;
};

class PropertyPanel : public Panel
{
  public:
    PropertyPanel();
    void SetEntity(Entity entity);
    void OnRenderUi() override;

  private:
    Entity mEntity;
};