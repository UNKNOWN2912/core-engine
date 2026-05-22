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
    None,
    DeferredAttachmentResize,
};

class Renderer
{
public:
    static void Initialize(const Window &window);
    static void Terminate();

    static void Submit(StaticMesh &mesh, Material &material);
    static void Submit(StaticMesh &mesh, Material &material, const Transform &transform);
    static void Submit(const RenderCommand &renderCommand);

    static void BeginFrame(RenderTarget &renderTarget, const Camera &camera = {});
    static void EndFrame();

    static bool ResizeSwapchain(const glm::uvec2 &size);
    static void DisplayToWindow(const RenderTarget &target);

    static const glm::uvec2 &GetSwapchainSize()
    {
        return mSwapchain.GetSize();
    }
    static const RenderPass &GetDeferredRenderPass();
    static const UniformBuffer &GetDeferredUniformBuffer()
    {
        return mDeferred.uniformBuffer;
    }

    static const Swapchain &GetSwapchain()
    {
        return mSwapchain;
    }
    static const DeferredAttachment &GetDeferredAttachments()
    {
        return mDeferred.attachment;
    }
    static const Sampler &GetDefaultSampler()
    {
        return mDefaultSampler;
    }

    static void AddListener(const std::function<bool(uint32_t, void *)> &listener);

    static void QueueSwapchainResize(const glm::uvec2 &size);

    static void DeferredPass();
    static void LightingPass();
    static void ResizeAttachments(const glm::uvec2 &size);

    static void GetBasicShader(VertexShaderID &outputVertexShader, FragmentShaderID &outputFragmentShader)
    {
        outputVertexShader = mBasicVertexShader;
        outputFragmentShader = mBasicFragmentShader;
    }

    static void SetBasicShader(VertexShaderID vertexShader, FragmentShaderID fragmentShader)
    {
        mBasicVertexShader = vertexShader;
        mBasicFragmentShader = fragmentShader;
    }

private:
    static VertexShaderID mBasicVertexShader;
    static FragmentShaderID mBasicFragmentShader;

    friend class EditorLayer;

    static struct Deferred
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

    static void CreateDeferredPassObjects();

    static struct Lighting
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

    static void CreateLightingPassObjects();

    static glm::uvec2 mSwapchainSize;

    static EventDispatcher mDispatcher;
    static GraphicsContext mContext;
    static Swapchain mSwapchain;

    static std::vector<RenderCommand> mRenderCommands;
    static RenderTarget mCurrentRenderTarget;

    static CommandBuffer mRenderCommandBuffer;
    static CommandBuffer mTransferToSwapchainCommandBuffer;

    static Semaphore mImageAcquiredSemaphore;
    static Semaphore mTransferSemaphore;
    static Semaphore mRenderingSemaphore;

    static Sampler mDefaultSampler;

    static FrameInfo mFrameInfo;

    friend class Editor;
};