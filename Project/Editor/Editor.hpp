#pragma once
#include <Engine.hpp>
#include "CameraController.hpp"
#include "imgui.h"

class EditorLayer : public Layer
{
    void OnAttach() override;
    void OnUpdate() override;
    void OnDetach() override;
};

class Editor : public Application
{
    void OnStart() override;
    void OnUpdate() override;
    void OnEnd() override;

    void OnKeyPress(Key key) override;
    void OnWindowResize(const glm::uvec2 &size) override;
    void OnMouseMove(const glm::vec2 &position, const glm::vec2 &offset) override;


    void InitializeImgui();
    void TerminateImgui();
    void RenderImgui();
    
    void RenderUi();
    void ResizeRenderView(const glm::uvec2& size);
    void UpdateCamera();

    Camera mCamera;
    CameraController mCameraController;

    RenderTarget mTarget;
    
    CommandBuffer mImGuiCommandBuffer;
    RenderPass mImguiRenderPass;
    Semaphore mImageAcquiredSemaphore;
    Semaphore mRenderingFinished;

    std::vector<FrameBuffer> mImguiFrameBuffer;

    ImTextureID mRenderViewTexture;

    glm::uvec2 mViewSize;

    Material skyboxMaterial;
    Material material;

    StaticMesh mesh;
    GraphicsPipeline pipeline;

    bool mColorCustomization = false;
    bool mEnableImageViewer = false;

    ImTextureID mImage;

    void AddImageView(const char* identifier, const Image& image);

    std::vector<const char*> imageIdentifiers;
    std::vector<Image> images;


    std::shared_ptr<EditorLayer> mEditorLayer;
};
