#pragma once
#include "Renderer/Camera.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "GraphicsContext.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Transform.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "Renderer/Utility.hpp"
#include <vulkan/vulkan_core.h>

struct Semaphores
{
    VkSemaphore imageAcquired = VK_NULL_HANDLE;
    VkSemaphore renderingFinish = VK_NULL_HANDLE;
};

struct CommandBuffers
{
    CommandBuffer render;
};

struct UniformData
{
    glm::mat4 projection;
    glm::mat4 view;
    alignas(16) glm::vec3 cameraPosition;
    alignas(16) glm::vec3 cameraFront;
    float time = 0;
};

struct MeshMap
{
    Material* material;
    Transform* transform;
};

struct DrawSubmitInfo
{
    StaticMesh* mesh = nullptr;
    Material* material = nullptr;
    Transform transform;
    InstanceBuffer* instanceBuffer = nullptr;
    bool instanced = false;
    uint32_t instanceCount = 0;
};

struct DeferredAttachment
{
    Image mPosition;
    Image mAlbedo;
    Image mNormal;
    Image mDepth;
    glm::uvec2 mSize;

    void ResizeAttachments(const glm::uvec2& size);
    void CreateAttachments(const glm::uvec2& size);
    void DestroyAttachments();

};

class Renderer
{
    public:
        void Initialize(const Window& window);
        void Terminate();

        void DrawMesh(StaticMesh& mesh);
        void DrawMeshWithMaterial(StaticMesh& mesh, Material& material, Transform transform);
        void DrawMeshWithMaterialInstanced(StaticMesh& mesh, Material& material, InstanceBuffer& instanceBuffer, uint32_t instanceCount);

        void BeginFrame();
        void EndFrame();

        void CreateMeshData(StaticMesh& mesh);

        void SetCamera(const Camera& camera) { mCamera = camera; }
        const Camera& GetCamera() const { return mCamera; }

        void Resize(const glm::uvec2& size);

        RenderPass GetMainRenderPass() const { return mRenderPass; }
        

        const UniformBuffer& GetUniformBuffer() const { return mUniformBuffer; }

    private:
        void CmdMainRenderPass(uint32_t imageIndex);

        void CreateRenderPass();
        void CreateSwapchainFramebuffers();
        void CreateSemaphores();
        void CreateCommandBuffers();
        
        void DestroyRenderPass();
        void DestroySwapchainFramebuffers();
        void DestroySemaphores();
        void DestroyCommandBuffers();

        void UpdateMaterialDescriptorSet(const std::vector<DrawSubmitInfo>& drawSubmitInfos, UniformBuffer& uniformBuffer, UniformData& uniformData);

        void CmdDrawSubmitBindDescriptorSet(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindPipeline(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindVertexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindIndexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);

        void RenderDrawSubmitInfos(const std::vector<DrawSubmitInfo>& drawSubmitInfos);

        void PresentImage(VkQueue queue, const Swapchain& swapchain, uint32_t imageIndex, VkSemaphore waitSemaphore);
        
        void UpdateUniformData();

        void CreateFinalImageAttachment(const glm::uvec2& size);
    private:
        GraphicsContext mContext;

        Swapchain mSwapchain;
        std::vector<VkFramebuffer> mSwapchainFramebuffer;

        VkFramebuffer mFinalFrameBuffer;
        Image mFinalImage;

        Semaphores mSemaphores;
        CommandBuffers mCommandBuffers;

        RenderPass mRenderPass;


        RenderPass mMainRenderPass;
        RenderPass mSwapchainRenderPass;


        VkViewport mViewport;

        std::vector<DrawSubmitInfo> mDrawSubmitInfo;

        bool mFrameRunning = false;

        UniformBuffer mUniformBuffer;
        UniformData mUniformData;

        Texture mTexture;

        Camera mCamera;

        Image mDepthAttachment;
        Image mSceneDepthAttachment;

        glm::vec3 lightDirection = glm::vec3(1,1,1);

        DeferredAttachment mDeferredAttachments;

        friend class Editor;
};