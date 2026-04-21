#pragma once
#include "Renderer/Camera.hpp"
#include "Renderer/CommandBuffer.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "GraphicsPipeline.hpp"
#include "GraphicsContext.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/Texture.hpp"
#include "Renderer/Transform.hpp"
#include "Renderer/UniformBuffer.hpp"

struct Swapchain
{
    uint32_t imageCount = 0;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    VkExtent2D extent = {0,0};
    VkSwapchainKHR handle = VK_NULL_HANDLE;
    std::vector<VkImage> images;
    std::vector<VkImageView> views;
};


struct Semaphores
{
    VkSemaphore imageAcquired = VK_NULL_HANDLE;
    VkSemaphore renderingFinish = VK_NULL_HANDLE;
};

struct CommandBuffers
{
    // VkCommandBuffer renderingCommandBuffer = VK_NULL_HANDLE;

    CommandBuffer render;
};

struct UniformData
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::vec3 cameraPosition;
    float     _pad1;  
    glm::vec3 cameraFront;
    float     _pad2;  
    float time = 0;
    float     _pad[3];
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

        VkRenderPass GetMainRenderPass() const { return mRenderPass.GetHandle(); }

    private:
        void CreateSwapchain(const glm::uvec2& size);
        void CreateRenderPass();
        void CreateSwapchainFramebuffers();
        void CreateSemaphores();
        void CreateCommandBuffers();
        
        void DestroySwapchain();
        void DestroyRenderPass();
        void DestroySwapchainFramebuffers();
        void DestroySemaphores();
        void DestroyCommandBuffers();

        void SetUniformCameraData(UniformData& data, const Camera& camera);
        void UpdateMaterialDescriptorSet(const std::vector<DrawSubmitInfo>& drawSubmitInfos, UniformBuffer& uniformBuffer, UniformData& uniformData);

        void CmdDrawSubmitBindDescriptorSet(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindPipeline(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindVertexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);
        void CmdDrawSubmitBindIndexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo);

        void RenderDrawSubmitInfos(const std::vector<DrawSubmitInfo>& drawSubmitInfos);

        void PresentImage(VkQueue queue, const Swapchain& swapchain, uint32_t imageIndex, VkSemaphore waitSemaphore);
        
    private:
        GraphicsContext mContext;

        Swapchain mSwapchain;
        std::vector<VkFramebuffer> mSwapchainFramebuffer;

        Semaphores mSemaphores;
        CommandBuffers mCommandBuffers;

        RenderPass mRenderPass;

        VkViewport mViewport;

        std::vector<DrawSubmitInfo> mDrawSubmitInfo;

        bool mFrameRunning = false;

        UniformBuffer mUniformBuffer;
        UniformData mUniformData;

        Texture mTexture;

        Camera mCamera;

        Image mDepthAttachment;
};