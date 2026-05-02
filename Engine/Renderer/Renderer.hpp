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

class Renderer
{
    public:
        void Initialize(const Window& window);
        void Terminate();

        void Submit(const StaticMesh& mesh, const Material& material);
        void Submit(const RenderCommand& renderCommand);

        void BeginFrame(RenderTarget& renderTarget, const Camera& camera = {});
        void EndFrame();

        void ResizeSwapchain(const glm::uvec2& size);
        void DisplayToWindow(const RenderTarget& target);

        const RenderPass& GetDeferredRenderPass() const;

        const UniformBuffer& GetRendererUniformBuffer() const { return mRendererUniformBuffer; }

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