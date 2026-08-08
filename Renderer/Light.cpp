#include "Assets/TextureManager.hpp"
#include "Renderer/Renderer.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Assets/ShaderManager.hpp"
#include "Light.hpp"
#include "Renderer/Helper.hpp"
#include "Renderer/ImageView.hpp"
#include "Renderer/Mesh.hpp"
#include <cstring>
#include <glm/gtc/matrix_transform.hpp>

void Light::Initialize()
{
    mUniformBuffer = UniformBuffer(sizeof(ShadowMapUniformData));

    Subpass subpass;
    subpass.SetDepthAttachment(0);

    mRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);
    mRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::EarlyFragmentTests, PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests);
    mRenderPass.CreateRenderPass();

    mUniformBuffer = UniformBuffer(sizeof(ShadowMapUniformData));

    mDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    mDescriptor.CreateDescriptor();
    mDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);

    std::string pointLightShader = ShaderManager::Load("point", "Shaders/shadow.vert.spv", "Shaders/shadow.frag.spv", "Shaders/shadow.geom.spv", "", false);

    mPointLightPipeline.SetVertexShader(ShaderManager::Get(pointLightShader).vertex);
    mPointLightPipeline.SetFragmentShader(ShaderManager::Get(pointLightShader).fragment);
    mPointLightPipeline.SetGeometryShader(ShaderManager::Get(pointLightShader).geometry);
    mPointLightPipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    mPointLightPipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mPointLightPipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    mPointLightPipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));
    mPointLightPipeline.EnableDepthTesting(true);
    mPointLightPipeline.EnableDepthWrite(true);
    mPointLightPipeline.AddDescriptors(mDescriptor);
    mPointLightPipeline.AddDescriptors(TextureManager::GetDescriptor());
    mPointLightPipeline.SetCullMode(CullMode::Back);
    mPointLightPipeline.SetPushConstant(ShaderStage::All, sizeof(PushConstantData));
    mPointLightPipeline.CreatePipeline(mRenderPass, 0);

    std::string directionalLightShader = ShaderManager::Load("directional", "Shaders/directional.vert.spv", "Shaders/directional.frag.spv", "Shaders/directional.geom.spv", "", false);

    mDirectionalShadowPipeline.SetVertexShader(ShaderManager::Get(directionalLightShader).vertex);
    mDirectionalShadowPipeline.SetFragmentShader(ShaderManager::Get(directionalLightShader).fragment);
    mDirectionalShadowPipeline.SetGeometryShader(ShaderManager::Get(directionalLightShader).geometry);
    mDirectionalShadowPipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    mDirectionalShadowPipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mDirectionalShadowPipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    mDirectionalShadowPipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));
    mDirectionalShadowPipeline.EnableDepthTesting(true);
    mDirectionalShadowPipeline.EnableDepthWrite(true);
    mDirectionalShadowPipeline.AddDescriptors(mDescriptor);
    mDirectionalShadowPipeline.AddDescriptors(TextureManager::GetDescriptor());
    mDirectionalShadowPipeline.SetCullMode(CullMode::None);
    mDirectionalShadowPipeline.SetPushConstant(ShaderStage::All, sizeof(PushConstantData));
    mDirectionalShadowPipeline.CreatePipeline(mRenderPass, 0);

    mCommandBuffer.CreateCommandBuffer();
    mSampler.CreateSampler();
}

void Light::Terminate()
{
    mCommandBuffer.DestroyCommandBuffer();
    mPointLightPipeline.DestroyPipeline();
    mDirectionalShadowPipeline.DestroyPipeline();
    mRenderPass.DestroyRenderPass();
    mDescriptor.DestroyDescriptor();
    mUniformBuffer.DestroyUniformBuffer();
    mSampler.DestroySampler();
}

void Light::GenerateShadowMap(const std::vector<RenderCommand> &renderCommand)
{
    switch (mType)
    {
    case LightType::DirectionalLight:
        GenerateDirectionalLightShadowMap(renderCommand);
        break;
    case LightType::PointLight:
        GeneratePointLightShadowMap(renderCommand);
        break;
    case LightType::SpotLight:
        GenerateSpotLightShadowMap(renderCommand);
        break;
    }
}

void Light::GeneratePointLightShadowMap(const std::vector<RenderCommand> &renderCommands)
{
    if (!mShadowMapOutdated)
    {
        return;
    }

    if (!mIsCubeMap)
    {
        mIsCubeMap = true;
        DestroyImage(mShadowMap);
        mFrameBuffers.clear();

        mShadowMap = CreateCubeMapImage(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler,
                                        ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
        FrameBuffer frameBuffer;
        frameBuffer.CreateFrameBuffer(std::initializer_list<ImageDeprecated>{mShadowMap}, mRenderPass, 6);
        mFrameBuffers.emplace_back(frameBuffer);
    }

    glm::vec3 front[6] =
        {
            glm::vec3(1, 0, 0),
            glm::vec3(-1, 0, 0),
            glm::vec3(0, 1, 0),
            glm::vec3(0, -1, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, -1),
        };

    glm::vec3 up[6] =
        {
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
            glm::vec3(0, 0, 1),
            glm::vec3(0, 0, -1),
            glm::vec3(0, -1, 0),
            glm::vec3(0, -1, 0),
        };

    ShadowMapUniformData data{};
    for (int i = 0; i < 6; i++)
    {
        data.projections[i] = GetPointProjection(front[i], up[i]);
    }
    data.position = mPosition;
    mUniformBuffer.SetData(&data);

    mCommandBuffer.BeginRecording();

    mRenderPass.CmdBeginRenderPass(mCommandBuffer, mFrameBuffers[0], mShadowMap.size, {{1.f, 1.f, 1.f, 1.f}});

    CmdBindDescriptors(mCommandBuffer, mPointLightPipeline, {&mDescriptor, &TextureManager::GetDescriptor()});
    mPointLightPipeline.CmdBindPipeline(mCommandBuffer);

    for (const RenderCommand &renderCommand : renderCommands)
    {
        ShadowPushConstant constant{};
        memcpy(&constant.model, renderCommand.pushContantData, sizeof(glm::mat4));
        constant.intensity = mIntensity;

        CmdBindVertexBuffers(mCommandBuffer, {*renderCommand.vertexBuffer});
        vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);

        VkViewport viewport =
            {
                .width = (float)mShadowMap.size.x,
                .height = (float)mShadowMap.size.y,
                .minDepth = 0.f,
                .maxDepth = 1.f,
            };

        VkRect2D scissor =
            {
                .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
            };

        vkCmdSetViewport(mCommandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(mCommandBuffer.GetHandle(), 0, 1, &scissor);
        vkCmdSetCullMode(mCommandBuffer.GetHandle(), VK_CULL_MODE_FRONT_BIT);
        vkCmdSetDepthTestEnable(mCommandBuffer.GetHandle(), true);
        vkCmdSetDepthWriteEnable(mCommandBuffer.GetHandle(), true);

        vkCmdPushConstants(mCommandBuffer.GetHandle(), mPointLightPipeline.GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, renderCommand.pushContantSize, renderCommand.pushContantData);
        vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
    }

    mRenderPass.CmdEndRenderPass(mCommandBuffer);

    mCommandBuffer.EndRecording();
    mCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics);
    vkDeviceWaitIdle(GraphicsContext::GetDevice());

    mShadowMapOutdated = false;
}
void Light::GenerateDirectionalLightShadowMap(const std::vector<RenderCommand> &renderCommands)
{
    // if (!mShadowMapOutdated)
    // {
    //     return;
    // }

    int cascadeCount = 4;
    if (mIsCubeMap || mShadowMap.handle == VK_NULL_HANDLE)
    {
        mIsCubeMap = false;
        DestroyImage(mShadowMap);
        mFrameBuffers.clear();
        mShadowMap = CreateImage(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler,
                                 ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One, cascadeCount);

        FrameBuffer frameBuffer;
        frameBuffer.CreateFrameBuffer(std::initializer_list<ImageDeprecated>{mShadowMap}, mRenderPass, 4);
        mFrameBuffers.emplace_back(frameBuffer);
    }

    ShadowMapUniformData data{};
    for (int i = 0; i < 4; i++)
    {
        data.projections[i] = GetDirectionalProjection(i);
    }
    data.position = mPosition;
    mUniformBuffer.SetData(&data);
    mDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);

    mCommandBuffer.BeginRecording();

    mRenderPass.CmdBeginRenderPass(mCommandBuffer, mFrameBuffers[0], mShadowMap.size, {{1.f, 1.f, 1.f, 1.f}});

    CmdBindDescriptors(mCommandBuffer, mDirectionalShadowPipeline, std::initializer_list<const Descriptor *>{&mDescriptor, &TextureManager::GetDescriptor()});
    mDirectionalShadowPipeline.CmdBindPipeline(mCommandBuffer);

    for (const RenderCommand &renderCommand : renderCommands)
    {
        CmdBindVertexBuffers(mCommandBuffer, {*renderCommand.vertexBuffer});
        vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);
        VkViewport viewport =
            {
                .width = (float)mShadowMap.size.x,
                .height = (float)mShadowMap.size.y,
                .minDepth = 0.f,
                .maxDepth = 1.f,
            };

        VkRect2D scissor =
            {
                .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
            };

        vkCmdSetViewport(mCommandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(mCommandBuffer.GetHandle(), 0, 1, &scissor);
        vkCmdSetCullMode(mCommandBuffer.GetHandle(), VK_CULL_MODE_FRONT_BIT);
        vkCmdSetDepthTestEnable(mCommandBuffer.GetHandle(), true);
        vkCmdSetDepthWriteEnable(mCommandBuffer.GetHandle(), true);
        vkCmdPushConstants(mCommandBuffer.GetHandle(), mDirectionalShadowPipeline.GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, renderCommand.pushContantSize, renderCommand.pushContantData);

        vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
    }

    mRenderPass.CmdEndRenderPass(mCommandBuffer);

    mCommandBuffer.EndRecording();
    mCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics);

    mShadowMapOutdated = false;
}
void Light::GenerateSpotLightShadowMap(const std::vector<RenderCommand> &renderCommand)
{
    if (!mShadowMapOutdated)
    {
        return;
    }

    if (mIsCubeMap)
    {
        DestroyImage(mShadowMap);

        mShadowMap = CreateImage(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler,
                                 ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
        mIsCubeMap = false;
    }
}

glm::mat4 Light::GetTightlyBoundedMatrix(const Camera &camera) const
{
    glm::mat4 inverse = glm::inverse(camera.GetProjection() * camera.GetView());

    uint32_t vertexCount = 8;
    glm::vec3 boxVertices[] =
        {
            glm::vec3(-1, 1, 0),
            glm::vec3(1, 1, 0),
            glm::vec3(1, -1, 0),
            glm::vec3(-1, -1, 0),

            glm::vec3(-1, 1, 1),
            glm::vec3(1, 1, 1),
            glm::vec3(1, -1, 1),
            glm::vec3(-1, -1, 1),
        };

    glm::vec3 center = glm::vec3(0);
    for (int i = 0; i < 8; i++)
    {
        glm::vec4 vertex = glm::vec4(boxVertices[i], 1);
        vertex = inverse * vertex;
        boxVertices[i] = glm::vec3(glm::vec3(vertex) / vertex.w);

        center += boxVertices[i];
    }

    center /= 8;

    float radius = glm::length(boxVertices[0] - boxVertices[6]);
    float texelsPerUnit = float(mShadowMapResolution) / (2 * radius);

    glm::mat4 scalar = glm::mat4(1.f);
    scalar = glm::scale(scalar, glm::vec3(texelsPerUnit));

    glm::mat4 tempView = glm::lookAt(glm::vec3(0), mDirection, glm::vec3(0, 1, 0));
    tempView = scalar * tempView;

    glm::mat4 viewInv = glm::inverse(tempView);

    center = tempView * glm::vec4(center, 1.0);
    center.x = glm::floor(center.x);
    center.y = glm::floor(center.y);
    center = viewInv * glm::vec4(center, 1.0);

    glm::vec3 eyes = center - (-mDirection * radius * 2.f);

    glm::mat4 view = glm::lookAt(eyes, center, glm::vec3(0, 1, 0));

    glm::vec3 position = mCamera.GetPosition();
    glm::mat4 projection = glm::ortho(-radius, radius, -radius, radius, -radius * 30.f, radius * 30.f);
    projection[1][1] *= -1;

    return projection * view;
}

const glm::vec3 &Light::GetPosition() const
{
    return mPosition;
}

const glm::vec3 &Light::GetDirection() const
{
    return mDirection;
}

const glm::vec3 &Light::GetColor() const
{
    return mColor;
}

void Light::SetPosition(const glm::vec3 &position)
{
    if (mPosition != position)
    {
        mShadowMapOutdated = true;
    }
    mPosition = position;
}

void Light::SetDirection(const glm::vec3 &direction)
{
    if (mDirection != direction)
    {
        mShadowMapOutdated = true;
    }

    mDirection = direction;
}

void Light::SetColor(const glm::vec3 &color)
{
    mColor = color;
}

float Light::GetOuterCosinAngle() const
{
    return mOuterCosinAngle;
}

float Light::GetInnerCosinAngle() const
{
    return mInnerCosinAngle;
}

float Light::GetIntensity() const
{
    return mIntensity;
}

void Light::SetOuterCosinAngle(float outerCosinAngle)
{
    if (mOuterCosinAngle != outerCosinAngle)
    {
        mShadowMapOutdated = true;
    }

    mOuterCosinAngle = outerCosinAngle;
}

void Light::SetInnerCosinAngle(float innerCosinAngle)
{
    if (mInnerCosinAngle != innerCosinAngle)
    {
        mShadowMapOutdated = true;
    }

    mInnerCosinAngle = innerCosinAngle;
}

void Light::SetIntensity(float intensity)
{
    if (mIntensity != intensity)
    {
        mShadowMapOutdated = true;
    }

    mIntensity = intensity;
}

void Light::SetType(LightType type)
{
    if (mType != type)
    {
        mShadowMapOutdated = true;
    }

    mType = type;
}

bool Light::IsShadowMapOutdated() const
{
    return mShadowMapOutdated;
}

glm::mat4 Light::GetDirectionalProjection(uint32_t cascadeIndex) const
{
    Camera nearCamera = mCamera;
    Camera midCamera = mCamera;
    Camera farMidCamera = mCamera;
    Camera farCamera = mCamera;

    nearCamera.SetNearPlane(0.01f);
    nearCamera.SetFarPlane(5.5f);

    midCamera.SetNearPlane(5.5f);
    midCamera.SetFarPlane(10.5f);

    farMidCamera.SetNearPlane(10.5f);
    farMidCamera.SetFarPlane(25.5f);

    farCamera.SetNearPlane(25.5f);
    farCamera.SetFarPlane(100.f);

    nearCamera.Calculate();
    midCamera.Calculate();
    farMidCamera.Calculate();
    farCamera.Calculate();

    Camera cameras[] = {nearCamera, midCamera, farMidCamera, farCamera};

    return GetTightlyBoundedMatrix(cameras[cascadeIndex]);
}

glm::mat4 Light::GetPointProjection(const glm::vec3 &front, const glm::vec3 &up) const
{
    glm::mat4 view = glm::lookAt(mPosition, mPosition + front, up);
    glm::mat4 projection = glm::perspective(glm::radians(90.f), 1.f, 0.2f, 100.f);
    return projection * view;
}

void Light::SetShadowMapResolution(uint32_t resolution)
{
    mShadowMapResolution = resolution;
}
uint32_t Light::GetShadowMapResolution() const
{
    return mShadowMapResolution;
}

const ImageDeprecated &Light::GetShadowMap() const
{
    return mShadowMap;
}

LightType Light::GetType() const
{
    return mType;
}

CommandBuffer Light::mCommandBuffer;
GraphicsPipeline Light::mPointLightPipeline;
GraphicsPipeline Light::mDirectionalShadowPipeline;
RenderPass Light::mRenderPass;
Descriptor Light::mDescriptor;
UniformBuffer Light::mUniformBuffer;
ShadowMapUniformData Light::mUniformData;
Sampler Light::mSampler;