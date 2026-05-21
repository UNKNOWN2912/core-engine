#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "EntityComponentSystem/EntityComponentSystem.hpp"
#include "Panel.hpp"

class EditorLayer : public Layer
{
  public:
    void OnAttach() override;
    void OnUpdate() override;
    void OnDetach() override;
    bool OnEvent(uint32_t code, void *data) override;

    void InitializeImGui();
    void TerminateImGui();
    void RenderImGui();

    void RenderUI();
    void SetRenderTarget(RenderTarget *renderTarget);
    void UpdateCamera();
    void MainMenuBar();

    void test();

    PanelManager mPanelManager;

    const Camera &GetEditorCamera() const;

    Scene *mScene;
    RenderPass mImGuiRenderPass;
    std::vector<FrameBuffer> mImGuiFrameBuffer;
    CommandBuffer mImGuiCommandBuffer;
    Semaphore mImageAcquiredSemaphore;
    Semaphore mRenderingFinished;

    RenderTarget *mRenderTarget;
    Camera mEditorCamera;
    CameraController mEditorCameraController;

    void CustomStyle();

    void EditMenu();

    void FileMenu();

    void ViewMenu();

    void WindowMenu();
};