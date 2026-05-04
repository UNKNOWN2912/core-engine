#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "Renderer/Renderer.hpp"
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
        void SetRenderTarget(RenderTarget* renderTarget) 
        {
            mRenderTarget = renderTarget;
        }
        const Camera& GetEditorCamera() const { return mEditorCamera; }

        void SetImage(const std::string& name, const Image& image);

        bool RendererEventCallback(uint32_t code, void* data);

        ImTextureID mImageViewId;
        std::unordered_map<std::string, Image> mImageMap;

        glm::uvec2 mViewSize = glm::uvec2(0);

        RenderPass mImGuiRenderPass;
        Renderer* mRenderer;
        std::vector<FrameBuffer> mImGuiFrameBuffer;

        CommandBuffer mImGuiCommandBuffer;

        Semaphore mImageAcquiredSemaphore;
        Semaphore mRenderingFinished;

        ImTextureID mRenderViewTexture;
        ImTextureID mImage;

        Sampler mSampler;

        RenderTarget* mRenderTarget;

        Camera mEditorCamera;
        CameraController mEditorCameraController;

        bool mEnableImageViewer = false;

        void ResizeRenderView(const glm::uvec2& size);

        void UpdateCamera();

        void SetImageView(const Image& image);


};