#pragma once
#include "glm/ext/vector_float2.hpp"
#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include <Engine.hpp>

class Editor : public Application
{
    void OnStart() override;
    void OnUpdate() override;

    void OnWindowResize(const glm::uvec2 &size) override;
    void OnKeyPress(Key key) override;

    void InitializeImgui();

    void CustomEndFrame();
    void CustomCmdMainRenderPass(uint32_t imageIndex);

    void StartImguiRender();
    void EndImguiRender();

    Material mMaterial;
    StaticMesh mMesh;

    ImTextureRef mFrameBuffer;
    VkSampler sampler;

    glm::uvec2 mGameViewSize = {0,0};

    void ResizeFrameBuffer(const glm::uvec2& size);

};
