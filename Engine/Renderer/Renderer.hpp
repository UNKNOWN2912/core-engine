#pragma once
#include "Core/Window.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/ComputePipeline.hpp"
#include "Renderer/InstanceBuffer.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/RenderTarget.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/Synchronization.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "RendererAttachments.hpp"

struct RenderCommand
{
    Buffer vertexBuffer;
    Buffer indexBuffer;

    InstanceBuffer instanceBuffer;
    uint32_t instanceCount = 0;
    
    GraphicsPipeline pipeline;
    Descriptor descriptors[16];

    uint32_t descriptorCount = 0;
    uint32_t indexCount = 0;
};

struct FrameInfo
{
    Camera camera;
    bool isRecording = false;
};

struct RendererUniformData
{
    glm::mat4 projection = glm::mat4(1.f);
    glm::mat4 view = glm::mat4(1.f);
    glm::vec3 cameraPosition;
};

enum class RendererEvent
{
    DeferredAttachmentResize,
};

class Renderer
{
    public:
        void Initialize(const Window& window);
        void Terminate();

        void Submit(const StaticMesh& mesh, const Material& material);
        void Submit(const RenderCommand& renderCommand);

        void BeginFrame(RenderTarget& renderTarget, const Camera& camera = {});
        void EndFrame();

        bool ResizeSwapchain(const glm::uvec2& size);
        void DisplayToWindow(const RenderTarget& target);

        const RenderPass& GetDeferredRenderPass() const;

        const UniformBuffer& GetRendererUniformBuffer() const { return mRendererUniformBuffer; }

        const Swapchain& GetSwapchain() const { return mSwapchain; }
        const DeferredSubpassAttachment& GetDeferredAttachments() const { return mDeferredAttachments; }
        const Sampler& GetDefaultSampler() const { return mDefaultSampler; }

        void AddListener(std::function<bool (uint32_t, void *)> listener);

        void QueueSwapchainResize(const glm::uvec2& size);

    private:
        // Render passes
        void CreateDeferredRenderPass();

        // Attachments
        void CreateAttachments(const glm::uvec2& size);
        void ResizeAttachments(const glm::uvec2& size);
        void DestroyAttachments();

        // FrameBuffer
        void CreateDeferredFrameBuffer(const glm::uvec2& size);

    private:
        glm::uvec2 mSwapchainSize;

        EventDispatcher mDispatcher;

        GraphicsContext mContext;

        RendererUniformData mRendererUniformData;
        UniformBuffer mRendererUniformBuffer;

        Swapchain mSwapchain;

        Descriptor mComputeDescriptor;
        Descriptor mDeferredAttachmentDescriptor;
        // Render passes
        RenderPass mDeferredRenderPass;
        
        // FrameBuffers
        FrameBuffer mDeferredFrameBuffer;
        
        // Attachments
        DeferredSubpassAttachment mDeferredAttachments;
        
        std::vector<RenderCommand> mRenderCommands;
        RenderTarget mCurrentRenderTarget;
        
        CommandBuffer mRenderCommandBuffer;
        CommandBuffer mTransferToSwapchainCommandBuffer;

        Semaphore mImageAcquiredSemaphore;
        Semaphore mTransferSemaphore;

        Sampler mDefaultSampler;

        ComputePipeline mComputePipeline;

        Semaphore mRenderingSemaphore;

        Image mComputeImage;

        FrameInfo mFrameInfo;
        
        friend class Editor;
};