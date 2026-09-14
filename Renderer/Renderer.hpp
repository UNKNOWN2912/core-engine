#pragma once
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
    ImageFormat presentationFormat = ImageFormat::BGRA8;
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
    float time = 0;
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
    Renderer() = default;
    Renderer(const Renderer &renderer) = delete;
    Renderer(const RendererSpecification &specification);
    ~Renderer();
    Renderer &operator=(const Renderer &renderer) = delete;
    Renderer &operator=(Renderer &&renderer);
    Renderer(Renderer &&renderer);

    void Move(Renderer &&renderer);

    void BeginFrame(const Camera &camera);
    void EndFrame(const glm::vec4 &clearColor = glm::vec4(1, 0, 1, 1));

    void SetResolution(const glm::uvec2 &resolution);
    const glm::uvec2 &GetResolution() const;
    SampleCount GetSampleCount() const;
    void SetSampleCount(const SampleCount &sampleCount);
    Surface CreateSurface(const Window &window, ImageFormat format = ImageFormat::BGRA8);
    void ResizeSurface(Surface &surface, ImageFormat format);
    void Present(Surface &surface);

    ShaderConfig GetSceneShaderConfig() const;

    const std::vector<RenderCommand> &GetRenderCommands() const;

    void Submit(RenderCommand renderCommand);
    void Submit(const Mesh &mesh, const Material &material, const Transform &transform, const TextureManager &textureManager, const ShaderManager &shaderManager);

    void SetBasicShader(std::string_view identifier, std::string_view vertexShader, std::string_view fragmentShader);

    std::string GetBasicShaderID() const;

    void AddLight(const Light &light);
    void ClearLights();

    void BeginLightPlacement();
    void EndLightPlacement();

    void SetProjectionMatrix(const glm::mat4 &matrix);
    void SetViewMatrix(const glm::mat4 &matrix);

    void CreateGraphicsPipeline(std::string_view identifier, ShaderManager &shaderManager);

    uint32_t GetInputInt() const;

    void SetInputInt(uint32_t inputInt);

    RenderPass &GetRenderPass();
    const RenderPass &GetRenderPass() const;
    const RenderPass &GetPresentRenderPass() const;

    const glm::uvec2 &GetViewportSize() const;
    void SetViewportSize(const glm::uvec2 &size);

    const Descriptor &GetBufferDescriptor() const
    {
        return mBufferDescriptor;
    }

    const Descriptor &GetShadowMapDescriptor() const
    {
        return mShadowMapDescriptor;
    }

    const RendererSpecification &GetSpecification() const
    {
        return Renderer::mSpecification;
    }

    const Descriptor &GetTextureDescriptor() const
    {
        return mTextureDescriptor;
    }

    uint32_t GetRenderPassColorSubpassIndex() const;

private:
    void Initialize(const RendererSpecification &specification);
    void Terminate();
    uint32_t mInputInt = 0;

    Descriptor mTextureDescriptor;
    Descriptor mBufferDescriptor;

    Sampler mSampler;
    FrameInfo mFrameInfo;
    RendererSpecification mSpecification;
    SampleCount mSampleCount = SampleCount::Four;
    glm::uvec2 mResolution = glm::uvec2(1920, 1080);
    RenderPass mSceneRenderPass;
    FrameBuffer mSceneFrameBuffer;

    ImageDeprecated mSceneColorAttachment;
    ImageDeprecated mSceneResolveAttachment;
    ImageDeprecated mSceneDepthAttachment;
    ImageDeprecated mSceneResolveDepthAttachment;

    CommandBuffer mCommandBuffer;
    Semaphore mImageAcquiredSemaphore;
    Semaphore mSwapchainRenderFinished;

    Shader mPresentShader;
    RenderPass mPresentRenderPass;
    CommandBuffer mPresentCommandBuffer;
    Descriptor mPresentInputDescriptor;

    UniformBuffer mUniformBuffer;
    UniformData mUniformData;

    std::vector<RenderCommand> mRenderCommands;

    std::vector<std::reference_wrapper<const Image>> mShadowMaps;

    std::vector<LightUniformData> mLight;
    StorageBuffer mLightStorageBuffer;
    Descriptor mShadowMapDescriptor;

    glm::uvec2 mViewportSize;

    Shader mDepthPrepassShader;

private:
    void CreateSceneRenderPassMultisampled();
    void CreateSceneFrameBufferMultisampled();
    void CreateSceneAttachmentsMultisampled();
    void CreateSceneRenderPass();
    void CreateSceneFrameBuffer();
    void CreateSceneAttachments();
    void CreatePresentPipeline();
    void CreatePresentRenderPass();

    void CmdDrawRenderCommand(const RenderCommand &renderCommand, const RenderCommand &previousCommand);

    friend class EditorUI;
};
