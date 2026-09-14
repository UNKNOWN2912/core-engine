#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "Light.hpp"
#include "Core/Application.hpp"
#include "Renderer/Helper.hpp"
#include "Renderer/ImageView.hpp"
#include "Renderer/Mesh.hpp"
#include "Renderer/Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>

void Light::Initialize()
{
    const Renderer &renderer = Application::GetInstance()->GetRenderer();
    mUniformBuffer = UniformBuffer(sizeof(ShadowMapUniformData));

    mCommandBuffer = CommandBuffer(GraphicsContext::GetCurrentContext().GetCommandPool());

    Subpass subpass;
    subpass.SetDepthAttachment(0);

    mRenderPass.AddSubpass(subpass, PipelineBindPoint::Graphic);
    mRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::None, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::EarlyFragmentTests, PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests);
    mRenderPass.CreateRenderPass();

    mUniformBuffer = UniformBuffer(sizeof(ShadowMapUniformData));

    mDescriptor.AddDescriptor(DescriptorType::Uniform, ShaderStage::Vertex);
    mDescriptor.Create();
    mDescriptor.UpdateBuffer(mUniformBuffer.GetBuffer(), 0);

    mPointLightPipeline = Shader("Shaders/shadow.vert.spv", "Shaders/shadow.frag.spv", mRenderPass, 0,
                                 ShaderConfig{
                                     .cullMode = CullMode::Back,
                                     .descriptors = {mDescriptor, renderer.GetTextureDescriptor()},
                                     .colorBlendAttachments = {false},
                                     .layouts = {Vertex::GetVertexLayout(0, 0)},
                                     .pushConstantSize = sizeof(ShadowPushConstant),
                                 });

    mDirectionalShadowPipeline = Shader("Shaders/directional.vert.spv", "Shaders/directional.frag.spv", mRenderPass, 0,
                                        ShaderConfig{
                                            .cullMode = CullMode::Back,
                                            .descriptors = {mDescriptor, renderer.GetTextureDescriptor()},
                                            .colorBlendAttachments = {0},
                                            .layouts = {Vertex::GetVertexLayout(0, 0)},
                                            .pushConstantSize = sizeof(ShadowPushConstant),
                                            .enableDepthBias = true,
                                            .slopeFactor = 1,
                                            .constantFactor = 5,
                                        });
}

void Light::Terminate()
{
    mRenderPass.DestroyRenderPass();
    mDescriptor.Destroy();
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
        // return;
    }

    if (!mIsCubeMap)
    {
        mIsCubeMap = true;
        mFrameBuffers.clear();
        mImageViews.clear();
        mShadowMap = Image::CreateCubeMap(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One, 1);

        for (uint32_t i = 0; i < 6; i++)
        {
            const ImageView &view = mImageViews.emplace_back(mShadowMap, ViewType::TwoDimensional, ImageAspect::Depth, i, 1);
            mFrameBuffers.emplace_back(mShadowMap.GetSize(), std::vector<std::reference_wrapper<const ImageView>>{view}, mRenderPass);
        }
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

    for (uint32_t i = 0; i < 6; i++)
    {
        mRenderPass.CmdBeginRenderPass(mCommandBuffer, mFrameBuffers[i], mShadowMap.GetSize(), {{1.f, 1.f, 1.f, 1.f}});

        CmdBindDescriptors(mCommandBuffer, mPointLightPipeline.GetGraphicsPipeline(), {&mDescriptor, &Application::GetInstance()->GetRenderer().GetTextureDescriptor()});
        mPointLightPipeline.GetGraphicsPipeline().CmdBindPipeline(mCommandBuffer);

        for (const RenderCommand &renderCommand : renderCommands)
        {
            PushConstantData *data = (PushConstantData *)&renderCommand.pushContantData[0];
            ShadowPushConstant pushConstant;
            pushConstant.model = data->model;
            pushConstant.intensity = mIntensity;
            pushConstant.projectionIndex = i;

            CmdBindVertexBuffers(mCommandBuffer, {*renderCommand.vertexBuffer});
            vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);

            VkViewport viewport =
                {
                    .width = (float)mShadowMap.GetSize().x,
                    .height = (float)mShadowMap.GetSize().y,
                    .minDepth = 0.f,
                    .maxDepth = 1.f,
                };

            VkRect2D scissor =
                {
                    .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
                };

            vkCmdSetViewport(mCommandBuffer.GetHandle(), 0, 1, &viewport);
            vkCmdSetScissor(mCommandBuffer.GetHandle(), 0, 1, &scissor);
            vkCmdSetCullMode(mCommandBuffer.GetHandle(), VK_CULL_MODE_NONE);
            vkCmdSetDepthTestEnable(mCommandBuffer.GetHandle(), true);
            vkCmdSetDepthWriteEnable(mCommandBuffer.GetHandle(), true);

            if (renderCommand.pushContantSize != 0)
                vkCmdPushConstants(mCommandBuffer.GetHandle(), mPointLightPipeline.GetGraphicsPipeline().GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, sizeof(pushConstant), &pushConstant);
            vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
        }

        mRenderPass.CmdEndRenderPass(mCommandBuffer);
    }

    mCommandBuffer.EndRecording();
    mCommandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().graphics);
    vkDeviceWaitIdle(GraphicsContext::GetCurrentContext().GetDevice());

    mShadowMapOutdated = false;
}

void Light::GenerateDirectionalLightShadowMap(const std::vector<RenderCommand> &renderCommands)
{
    if (!mShadowMapOutdated)
    {
        // return;
    }

    int cascadeCount = 4;
    if (mIsCubeMap || mShadowMap.GetHandle() == VK_NULL_HANDLE)
    {
        mIsCubeMap = false;
        mFrameBuffers.clear();
        mShadowMap = Image(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler,
                           ImageType::TwoDimensional, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One, cascadeCount);

        mFrameBuffers.clear();
        mImageViews.clear();

        for (int i = 0; i < cascadeCount; i++)
        {
            const ImageView &view = mImageViews.emplace_back(mShadowMap, ViewType::TwoDimensional, ImageAspect::Depth, i, 1);
            mFrameBuffers.emplace_back(mShadowMap.GetSize(), std::vector<std::reference_wrapper<const ImageView>>{view}, mRenderPass);
        }
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

    for (int i = 0; i < cascadeCount; i++)
    {
        mRenderPass.CmdBeginRenderPass(mCommandBuffer, mFrameBuffers[i], mShadowMap.GetSize(), {{1.f, 1.f, 1.f, 1.f}});

        CmdBindDescriptors(mCommandBuffer, mDirectionalShadowPipeline.GetGraphicsPipeline(), std::initializer_list<const Descriptor *>{&mDescriptor, &Application::GetInstance()->GetRenderer().GetTextureDescriptor()});
        mDirectionalShadowPipeline.GetGraphicsPipeline().CmdBindPipeline(mCommandBuffer);

        for (const RenderCommand &renderCommand : renderCommands)
        {
            PushConstantData *data = (PushConstantData *)&renderCommand.pushContantData[0];
            ShadowPushConstant pushConstant;
            pushConstant.model = data->model;
            pushConstant.intensity = mIntensity;
            pushConstant.projectionIndex = i;

            CmdBindVertexBuffers(mCommandBuffer, {*renderCommand.vertexBuffer});
            vkCmdBindIndexBuffer(mCommandBuffer.GetHandle(), renderCommand.indexBuffer->handle, 0, VK_INDEX_TYPE_UINT32);
            VkViewport viewport =
                {
                    .width = (float)mShadowMap.GetSize().x,
                    .height = (float)mShadowMap.GetSize().y,
                    .minDepth = 0.f,
                    .maxDepth = 1.f,
                };

            VkRect2D scissor =
                {
                    .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
                };

            vkCmdSetViewport(mCommandBuffer.GetHandle(), 0, 1, &viewport);
            vkCmdSetScissor(mCommandBuffer.GetHandle(), 0, 1, &scissor);
            vkCmdSetCullMode(mCommandBuffer.GetHandle(), VK_CULL_MODE_NONE);
            vkCmdSetDepthTestEnable(mCommandBuffer.GetHandle(), true);
            vkCmdSetDepthWriteEnable(mCommandBuffer.GetHandle(), true);
            vkCmdPushConstants(mCommandBuffer.GetHandle(), mDirectionalShadowPipeline.GetGraphicsPipeline().GetPipelineLayout(), VK_SHADER_STAGE_ALL, 0, sizeof(pushConstant), &pushConstant);

            vkCmdDrawIndexed(mCommandBuffer.GetHandle(), renderCommand.indexCount, 1, 0, 0, 0);
        }

        mRenderPass.CmdEndRenderPass(mCommandBuffer);
    }

    mCommandBuffer.EndRecording();
    mCommandBuffer.QueueSubmit(GraphicsContext::GetCurrentContext().GetQueues().graphics);

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
        mShadowMap = Image(glm::uvec2(mShadowMapResolution), ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageType::TwoDimensional, ImageAspect::Depth, MemoryProperty::DeviceLocal, SampleCount::One);
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

void Light::SetCamera(const Camera &camera)
{
    mCamera = camera;
    mShadowMapOutdated = true;
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

const Image &Light::GetShadowMap() const
{
    return mShadowMap;
}

LightType Light::GetType() const
{
    return mType;
}

CommandBuffer Light::mCommandBuffer;
Shader Light::mPointLightPipeline;
Shader Light::mDirectionalShadowPipeline;
RenderPass Light::mRenderPass;
Descriptor Light::mDescriptor;
UniformBuffer Light::mUniformBuffer;
ShadowMapUniformData Light::mUniformData;
Sampler Light::mSampler;