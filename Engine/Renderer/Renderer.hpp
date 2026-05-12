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
#include "Renderer/Transform.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "RendererAttachments.hpp"

struct RenderCommand
{
    Buffer* vertexBuffer;
    Buffer* indexBuffer;
    uint32_t indexCount = 0;

    InstanceBuffer* instanceBuffer;
    uint32_t instanceCount = 0;
    
    GraphicsPipeline* pipeline;
    Descriptor* descriptors[16];
    uint32_t descriptorCount = 0;

    std::byte pushContantData[128];
    size_t pushContantSize = 0;
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

        void Submit(StaticMesh& mesh, Material& material);
        void Submit(StaticMesh& mesh, Material& material, const Transform& transform);
        void Submit(const RenderCommand& renderCommand);

        void BeginFrame(RenderTarget& renderTarget, const Camera& camera = {});
        void EndFrame();

        bool ResizeSwapchain(const glm::uvec2& size);
        void DisplayToWindow(const RenderTarget& target);

        const RenderPass& GetDeferredRenderPass() const;

        const UniformBuffer& GetRendererUniformBuffer() const { return mRendererUniformBuffer; }

        const Swapchain& GetSwapchain() const { return mSwapchain; }
        const DeferredAttachment& GetDeferredAttachments() const { return mDeferred.attachment; }
        const Sampler& GetDefaultSampler() const { return mDefaultSampler; }

        void AddListener(std::function<bool (uint32_t, void *)> listener);

        void QueueSwapchainResize(const glm::uvec2& size);

        ~Renderer()
        {
            Terminate();
        }

        void DeferredPass();

        void LightingPass();

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

        friend class EditorLayer;

        struct Skybox
        {
            RenderPass renderPass;
            GraphicsPipeline graphicPipeline;
            FrameBuffer frameBuffer;
        } mSkybox;
        void CreateSkyboxPassObjects();

        struct Deferred
        {
            FrameBuffer frameBuffer;
            RenderPass renderPass;
            DeferredAttachment attachment;
        } mDeferred;
        void CreateDeferredPassObjects();
        
        struct Lighting
        {
            Descriptor descriptor;
            ComputePipeline pipeline;
            Image image;

            UniformBuffer uniformBuffer;

            struct UniformData
            {
                glm::vec3 cameraPosition;
            } uniformData;

            Descriptor uniformDescriptor;

        } mLighting;

        void CreateLightingPassObjects();

        void SkyboxPass();

        glm::uvec2 mSwapchainSize;

        EventDispatcher mDispatcher;

        GraphicsContext mContext;

        RendererUniformData mRendererUniformData;
        UniformBuffer mRendererUniformBuffer;

        Swapchain mSwapchain;

        Descriptor mDeferredAttachmentDescriptor;
        // Render passes
        
        // FrameBuffers
        
        // Attachments
        
        std::vector<RenderCommand> mRenderCommands;
        RenderTarget mCurrentRenderTarget;
        
        CommandBuffer mRenderCommandBuffer;
        CommandBuffer mTransferToSwapchainCommandBuffer;

        Semaphore mImageAcquiredSemaphore;
        Semaphore mTransferSemaphore;

        Sampler mDefaultSampler;


        Semaphore mRenderingSemaphore;


        FrameInfo mFrameInfo;
        
        friend class Editor;
};