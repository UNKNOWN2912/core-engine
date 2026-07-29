#pragma once
#include "Renderer/Camera.hpp"
#include "Renderer/ImageView.hpp"
#include "Renderer/RendererType.hpp"
#include "Renderer/UniformBuffer.hpp"
#include "Renderer/Utility.hpp"
#include <glm/glm.hpp>

enum class LightType
{
    DirectionalLight,
    PointLight,
    SpotLight
};

struct ShadowMapUniformData
{
    glm::mat4 projections[6];
    glm::vec3 position;
};

struct ShadowPushConstant
{
    glm::mat4 model;
    float intensity;
};

class Light
{
public:
    static void Initialize();
    static void Terminate();

    const glm::vec3 &GetPosition() const;
    const glm::vec3 &GetDirection() const;
    const glm::vec3 &GetColor() const;
    uint32_t GetShadowMapResolution() const;
    float GetOuterCosinAngle() const;
    float GetInnerCosinAngle() const;
    float GetIntensity() const;
    const ImageDeprecated &GetShadowMap() const;
    LightType GetType() const;

    void SetPosition(const glm::vec3 &position);
    void SetDirection(const glm::vec3 &direction);
    void SetColor(const glm::vec3 &color);
    void SetOuterCosinAngle(float outerCosinAngle);
    void SetInnerCosinAngle(float innerCosinAngle);
    void SetShadowMapResolution(uint32_t resolution);
    void SetIntensity(float intensity);
    void SetType(LightType type);
    void SetCamera(const Camera &camera)
    {
        mCamera = camera;
        mShadowMapOutdated = true;
    }

    void GenerateShadowMap(const std::vector<RenderCommand> &renderCommand);

    bool IsShadowMapOutdated() const;

    glm::mat4 GetDirectionalProjection(uint32_t cascadeIndex) const;
    glm::mat4 GetPointProjection(const glm::vec3 &front, const glm::vec3 &up) const;

private:
    friend class Editor;
    glm::vec3 mPosition = glm::vec3(0, 0, 0);
    glm::vec3 mDirection = glm::vec3(0, -1, 0);
    glm::vec3 mColor = glm::vec3(1, 1, 1);

    float mOuterCosinAngle = 0.5;
    float mInnerCosinAngle = 0.64278761;
    float mIntensity = 1;

    bool mShadowMapOutdated = true;
    bool mShadowMapCreated = false;
    bool mIsCubeMap = false;

    ImageDeprecated mShadowMap;
    LightType mType = LightType::PointLight;

    static CommandBuffer mCommandBuffer;
    static GraphicsPipeline mPointLightPipeline;
    static GraphicsPipeline mDirectionalShadowPipeline;
    static RenderPass mRenderPass;
    static Descriptor mDescriptor;
    static UniformBuffer mUniformBuffer;
    static ShadowMapUniformData mUniformData;
    static Sampler mSampler;

    Camera mCamera;
    bool mUseTightMatrix = false;

    std::vector<FrameBuffer> mFrameBuffers;
    std::vector<ImageView> mImageViews;

    uint32_t mShadowMapResolution = 2048;

private:
    void GeneratePointLightShadowMap(const std::vector<RenderCommand> &renderCommand);
    void GenerateDirectionalLightShadowMap(const std::vector<RenderCommand> &renderCommand);
    void GenerateSpotLightShadowMap(const std::vector<RenderCommand> &renderCommand);

    glm::mat4 GetTightlyBoundedMatrix(const Camera &camera) const;
};