#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "EntityComponentSystem.hpp"
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
        void ResizeRenderView(const glm::uvec2& size);
        void UpdateCamera();
        void CustomStyle();
        void CustomizationWindow();
        void StoreImGuiStyle(std::string_view filename, const ImGuiStyle& style);
        void LoadImGuiStyle(std::string_view filename, ImGuiStyle& style);
        void MainMenuBar();
        void GameView();
        void ControlPanel();
        void EntityPanel();

        const Camera &GetEditorCamera() const;

        bool mPropertyPanelEnable = true;
        bool mCustomizeWindowEnable = false;
        bool mDemoWindowEnable = false;
        bool mContentPanelEnable = true;
        bool mGameViewEnable = true;
        bool mEntityPanelEnable = true;


        bool mDisableCursor = false;

        Scene* mScene;
        Entity mSelectedEntity;

        glm::uvec2 mViewSize = glm::uvec2(0);

        RenderPass mImGuiRenderPass;
        std::vector<FrameBuffer> mImGuiFrameBuffer;
        CommandBuffer mImGuiCommandBuffer;
        Semaphore mImageAcquiredSemaphore;
        Semaphore mRenderingFinished;
        Sampler mSampler;
        ImTextureID mRenderViewTexture;
        RenderTarget* mRenderTarget;

        Camera mEditorCamera;
        CameraController mEditorCameraController;

        void PropertyPanel();

        void LoadState(std::string_view filename);

        void StoreState(std::string_view filename);
};