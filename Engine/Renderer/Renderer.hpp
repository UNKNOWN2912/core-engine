#pragma once
#include "Renderer/Camera.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "GraphicsPipeline.hpp"
#include "GraphicsContext.hpp"
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
    VkCommandBuffer renderingCommandBuffer = VK_NULL_HANDLE;
};

struct UniformData
{
    glm::mat4 projection;
    glm::mat4 view;
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
    Transform* transform = nullptr;
};

class Renderer
{
    public:
        void Initialize(const Window& window);
        void Terminate();

        void DrawMesh(StaticMesh& mesh);
        void DrawMeshWithMaterial(StaticMesh& mesh, Material& material, Transform& transform);

        void BeginFrame();
        void EndFrame();

        void CreateMeshData(StaticMesh& mesh);

        void SetCamera(const Camera& camera) { mCamera = camera; }
        const Camera& GetCamera() const { return mCamera; }

        void Resize(const glm::uvec2& size);

        VkRenderPass GetMainRenderPass() const { return mRenderPass; }

    private:
        void CreateSwapchain(const glm::uvec2& size);
        void CreateRenderPass();
        void CreateSwapchainFramebuffers();
        void CreateSemaphores();
        void CreateCommandBuffers();
        void CreatePipelines();
        
        void DestroySwapchain();
        void DestroyRenderPass();
        void DestroySwapchainFramebuffers();
        void DestroySemaphores();
        void DestroyCommandBuffers();

        
    private:
        GraphicsContext mContext;

        Swapchain mSwapchain;
        Semaphores mSemaphores;
        CommandBuffers mCommandBuffers;
        GraphicsPipeline mDefaultPipeline;

        VkRenderPass mRenderPass;
        VkViewport mViewport;

        std::vector<VkFramebuffer> mSwapchainFramebuffer;

        std::vector<DrawSubmitInfo> mDrawSubmitInfo;

        bool mFrameRunning = false;

        UniformBuffer mUniformBuffer;
        UniformData mUniformData;

        Texture mTexture;

        Camera mCamera;

        Image mDepthAttachment;
};