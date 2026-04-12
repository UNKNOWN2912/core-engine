#pragma once
#include "Renderer/Mesh.hpp"
#include "GraphicsPipeline.hpp"
#include "GraphicsContext.hpp"

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

class Renderer
{
    public:
        void Initialize(const Window& window);
        void Terminate();

        void DrawMesh(StaticMesh& mesh);

        void BeginFrame();
        void EndFrame();

        void CreateMeshData(StaticMesh& mesh);

    private:
        void CreateSwapchain();
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
        VkRenderPass mRenderPass;
        std::vector<VkFramebuffer> mSwapchainFramebuffer;
        std::vector<StaticMesh*> mMeshQueue;
        bool mFrameRunning = false;

        VkViewport mViewport;

        GraphicsPipeline mDefaultPipeline;
};