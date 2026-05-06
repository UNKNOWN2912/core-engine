#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "imgui.h"


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
        const Camera &GetEditorCamera() const;

        glm::uvec2 mViewSize = glm::uvec2(0);

        RenderPass mImGuiRenderPass;
        std::vector<FrameBuffer> mImGuiFrameBuffer;

        CommandBuffer mImGuiCommandBuffer;

        Semaphore mImageAcquiredSemaphore;
        Semaphore mRenderingFinished;

        ImTextureID mRenderViewTexture;

        Sampler mSampler;

        RenderTarget* mRenderTarget;

        Camera mEditorCamera;
        CameraController mEditorCameraController;


        void ResizeRenderView(const glm::uvec2& size);

        void UpdateCamera();

        void CustomStyle();

        void CustomizationWindow();

        bool mCustomizeWindowEnable = false;
        bool mDemoWindowEnable = false;
        bool mContentPanelEnable = true;
        bool mGameViewEnable = true;

        void StoreImGuiStyle(std::string_view filename, const ImGuiStyle& style);
        void LoadImGuiStyle(std::string_view filename, ImGuiStyle& style);

        void MainMenuBar();

        void GameView();

        void ControlPanel();

};