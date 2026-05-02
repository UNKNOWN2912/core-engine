#pragma once
#include <Engine.hpp>
#include "imgui.h"


class Editor : public Application
{
    void OnStart() override;
    void OnUpdate() override;
    void OnEnd() override;

    void OnKeyPress(Key key) override;
    void OnWindowResize(const glm::uvec2 &size) override;

    Material material;
    StaticMesh mesh;
    GraphicsPipeline pipeline;

    void InitializeImgui();
    void TerminateImgui();
    void RenderImgui();
    
    RenderTarget mTarget;
    RenderCommand renderCommand;
    CommandBuffer mImGuiCommandBuffer;
    RenderPass mImguiRenderPass;
    Semaphore mImageAcquiredSemaphore;
    Semaphore mRenderingFinished;

    std::vector<FrameBuffer> mImguiFrameBuffer;

    ImTextureID mRenderViewTexture;

    glm::uvec2 mViewSize;

    void RenderUi();


};
