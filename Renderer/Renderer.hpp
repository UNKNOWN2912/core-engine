#pragma once
#include "Assets/MaterialManager.hpp"
#include "Assets/MeshManager.hpp"
#include "Light.hpp"
#include "Renderer/Camera.hpp"
#include "Renderer/GraphicsPipeline.hpp"
#include "Renderer/Material.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/RenderPass.hpp"
#include "Renderer/StorageBuffer.hpp"
#include "Renderer/Swapchain.hpp"
#include "Renderer/Transform.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "RendererType.hpp"
#include <unordered_map>

const uint32_t maxLightCount = 1000;

struct FrameInfo
{
    bool recording = false;
};

struct RendererSpecification
{
    DeviceType deviceType = DeviceType::Dedicated;
};

struct Surface
{
    VkSurfaceKHR handle = VK_NULL_HANDLE;
    Swapchain swapchain;
    std::vector<FrameBuffer> frameBuffers;
};

struct RendererMaterialObject
{
    GraphicsPipeline pipeline;
    Descriptor textureDescriptor;
    Descriptor bufferDescriptor;
    Descriptor shadowMapDescriptor;
    Sampler sampler;
};

struct UniformData
{
    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 projection = glm::mat4(1.f);
    glm::mat4 directionalMatrix1 = glm::mat4(1.f);
    glm::mat4 directionalMatrix2 = glm::mat4(1.f);
    glm::mat4 directionalMatrix3 = glm::mat4(1.f);
    glm::mat4 directionalMatrix4 = glm::mat4(1.f);
    glm::vec3 cameraPosition = glm::vec3(0);
    int lightCount = 0;
    glm::vec3 cameraFront = glm::vec3(0);
    float time;
};

struct PushConstantData
{
    glm::mat4 model = glm::mat4(1.f);
    uint32_t albedoIndex = 0;
    uint32_t normalIndex = 0;
    uint32_t roughnessIndex = 0;
    uint32_t metallicIndex = 0;
    uint32_t inputInt = 0;
    float roughness = 0;
    float metallic = 0;
    float indexOfRefraction = 0;
    glm::vec4 color = glm::vec4(1);
};

struct LightUniformData
{
    glm::vec3 position;
    float intensity;

    glm::vec3 color;
    float innerAngle;

    glm::vec3 direction;
    float outerAngle;

    float radius;
    int type;
    int shadowMapIndex;
    int castShadow;
};

class Renderer
{
public:
    static void Initialize(const RendererSpecification &specification);
    static void Terminate();

    static void BeginFrame(const Camera &camera);
    static void EndFrame();

    static void SetResolution(const glm::uvec2 &resolution)
    {
        mResolution = resolution;
    }
    static const glm::uvec2 &GetResolution();
    static SampleCount GetSampleCount();
    static void SetSampleCount(const SampleCount &sampleCount);
    static Surface CreateSurface(const Window &window, ImageFormat format = ImageFormat::BGRA8);
    static void ResizeSurface(Surface &surface);
    static void Present(Surface &surface);

    static const std::vector<RenderCommand> &GetRenderCommands();

    static void Submit(RenderCommand renderCommand);
    static void Submit(const Mesh &mesh, const Material &material, const Transform &transform);
    static void Submit(MaterialID material, MeshID mesh, const Transform &transform);

    static void SetBasicShader(std::string_view vertexShader, std::string_view fragmentShader);

    static ShaderID GetBasicShaderID();

    static void AddLight(const Light &light);
    static void ClearLights();

    static void BeginLightPlacement();
    static void EndLightPlacement();

    static void SetProjectionMatrix(const glm::mat4 &matrix);
    static void SetViewMatrix(const glm::mat4 &matrix);

    static void CreateGraphicsPipeline(ShaderID id);

    static uint32_t GetInputInt()
    {
        return mInputInt;
    }

    static void SetInputInt(uint32_t inputInt)
    {
        mInputInt = inputInt;
    }

    static RenderPass &GetRenderPass();
    static const RenderPass &GetPresentRenderPass();

    static const glm::uvec2 &GetViewportSize();
    static void SetViewportSize(const glm::uvec2 &size);

    static const Descriptor &GetBufferDescriptor()
    {
        return mBufferDescriptor;
    }

private:
    static uint32_t mInputInt;

    static Descriptor mBufferDescriptor;

    static Sampler mSampler;
    static FrameInfo mFrameInfo;
    static RendererSpecification mSpecification;
    static SampleCount mSampleCount;
    static glm::uvec2 mResolution;
    static RenderPass mSceneRenderPass;
    static FrameBuffer mSceneFrameBuffer;

    static ImageDeprecated mSceneColorAttachment;
    static ImageDeprecated mSceneResolveAttachment;
    static ImageDeprecated mSceneDepthAttachment;
    static ImageDeprecated mSceneResolveDepthAttachment;

    static CommandBuffer mCommandBuffer;
    static Semaphore mImageAcquiredSemaphore;
    static Semaphore mSwapchainRenderFinished;

    static GraphicsPipeline mPresentPipeline;
    static RenderPass mPresentRenderPass;
    static CommandBuffer mPresentCommandBuffer;
    static Descriptor mPresentInputDescriptor;

    static UniformBuffer mUniformBuffer;
    static UniformData mUniformData;

    static std::vector<RenderCommand> mRenderCommands;

    static Camera mCamera;

    static std::vector<ImageDeprecated> mShadowMaps;
    static std::vector<LightUniformData> mLight;
    static StorageBuffer mLightStorageBuffer;

    static Descriptor mShadowMapDescriptor;

    static std::unordered_map<ShaderID, GraphicsPipeline> mShaderPipelineMap;
    static ShaderID mBasicShaderID;

    static glm::uvec2 mViewportSize;

private:
    static void CreateSceneRenderPassMultisampled();
    static void CreateSceneFrameBufferMultisampled();
    static void CreateSceneAttachmentsMultisampled();
    static void CreateSceneRenderPass();
    static void CreateSceneFrameBuffer();
    static void CreateSceneAttachments();
    static void CreatePresentPipeline();
    static void CreatePresentRenderPass();

    static void CmdDrawRenderCommand(const RenderCommand &renderCommand, const RenderCommand &previousCommand);

    friend class Editor;
};
