#pragma once
#include "CameraController.hpp"
#include "Core/LayerStack.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/Renderer.hpp"

class GameLayer : public Layer
{
    public:
        void OnAttach() override;
        void OnUpdate() override;
        void OnDetach() override;

        void UpdateCamera();

        void DestroyMaterial();

        void CreateMaterial();

        Camera mCamera;
        CameraController mCameraController;

        RenderTarget mTarget;
        
        CommandBuffer mImGuiCommandBuffer;
        RenderPass mImguiRenderPass;
        Semaphore mImageAcquiredSemaphore;
        Semaphore mRenderingFinished;


        Material skyboxMaterial;
        Material material;

        CommandBuffer commandBuffer;


        StaticMesh mesh;

};