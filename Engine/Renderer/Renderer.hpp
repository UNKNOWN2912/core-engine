#pragma once
#include "Assets/ShaderManager.hpp"
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
    Buffer *vertexBuffer;
    Buffer *indexBuffer;
    uint32_t indexCount = 0;

    InstanceBuffer *instanceBuffer;
    uint32_t instanceCount = 0;

    GraphicsPipeline *pipeline;
    Descriptor *descriptors[16];
    uint32_t descriptorCount = 0;

    std::byte pushContantData[128];
    size_t pushContantSize = 0;
};

struct FrameInfo
{
    Camera camera;
    bool isRecording = false;
};

enum class RendererEvent
{
    DeferredAttachmentResize,
};

class Renderer
{
  public:
    void Initialize(const Window &window);
    void Terminate();

    void Submit(StaticMesh &mesh, Material &material);
    void Submit(StaticMesh &mesh, Material &material, const Transform &transform);
    void Submit(const RenderCommand &renderCommand);

    void BeginFrame(RenderTarget &renderTarget, const Camera &camera = {});
    void EndFrame();

    bool ResizeSwapchain(const glm::uvec2 &size);
    void DisplayToWindow(const RenderTarget &target);

    const RenderPass &GetDeferredRenderPass() const;

    const UniformBuffer &GetDeferredUniformBuffer() const
    {
        return mDeferred.uniformBuffer;
    }

    const Swapchain &GetSwapchain() const
    {
        return mSwapchain;
    }
    const DeferredAttachment &GetDeferredAttachments() const
    {
        return mDeferred.attachment;
    }
    const Sampler &GetDefaultSampler() const
    {
        return mDefaultSampler;
    }

    void AddListener(std::function<bool(uint32_t, void *)> listener);

    void QueueSwapchainResize(const glm::uvec2 &size);

    ~Renderer()
    {
        Terminate();
    }

    void DeferredPass();
    void LightingPass();
    void ResizeAttachments(const glm::uvec2 &size);

    void GetBasicShader(VertexShaderID &outputVertexShader, FragmentShaderID &outputFragmentShader)
    {
        outputVertexShader = mBasicVertexShader;
        outputFragmentShader = mBasicFragmentShader;
    }

    void SetBasicShader(VertexShaderID vertexShader, FragmentShaderID fragmentShader)
    {
        mBasicVertexShader = vertexShader;
        mBasicFragmentShader = fragmentShader;
    }

  private:
    VertexShaderID mBasicVertexShader;
    FragmentShaderID mBasicFragmentShader;

    friend class EditorLayer;

    struct Deferred
    {
        FrameBuffer frameBuffer;
        RenderPass renderPass;
        DeferredAttachment attachment;
        Descriptor descriptor;

        struct UniformData
        {
            glm::mat4 projection = glm::mat4(1.f);
            glm::mat4 view = glm::mat4(1.f);
            glm::vec3 cameraPosition;
        } uniformData;

        UniformBuffer uniformBuffer;
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

    glm::uvec2 mSwapchainSize;

    EventDispatcher mDispatcher;
    GraphicsContext mContext;
    Swapchain mSwapchain;

    std::vector<RenderCommand> mRenderCommands;
    RenderTarget mCurrentRenderTarget;

    CommandBuffer mRenderCommandBuffer;
    CommandBuffer mTransferToSwapchainCommandBuffer;

    Semaphore mImageAcquiredSemaphore;
    Semaphore mTransferSemaphore;
    Semaphore mRenderingSemaphore;

    Sampler mDefaultSampler;

    FrameInfo mFrameInfo;

    friend class Editor;
};