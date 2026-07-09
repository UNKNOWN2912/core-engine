#include "Renderer/Converter.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "Assets/ModelImporter.hpp"
#include "CameraController.hpp"
#include "EntityComponentSystem/Component.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include <Engine.hpp>

#define BindCommandCallback(callback) std::bind(&callback, this, std::placeholders::_1)

class RendererRework : public Application
{
    Surface mSurface;
    Camera mCamera;
    CameraController mController;
    Scene mScene;
    std::vector<Light *> mLights;

    void OnInitialize() override
    {
        int scale = 240;
        Renderer::SetResolution({16 * scale, 9 * scale});
    }

    void OnStart() override
    {
        mCamera.SetFov(90.f);

        HideCursor();

        mController.SetSensitivity(0.1f);

        GetWindow().SetTitle("Renderer Rework");
        GetWindow().SetFullscreen(true);

        Renderer::SetBasicShader("Shaders/physical.vert.spv", "Shaders/physical.frag.spv");

        mController.SetCamera(mCamera, GetWindow());

        mSurface = Renderer::CreateSurface(GetWindow());

        ModelImporter importer;
        importer.Import("Models/cube/cube.gltf", mScene);
        importer.Import("Models/Sponza/Sponza.gltf", mScene);

        std::shared_ptr<Material> skyboxMaterial = std::make_shared<Material>();
        skyboxMaterial->shader = ShaderManager2::Load("Shaders/skybox.vert.spv", "Shaders/skybox.frag.spv");
        skyboxMaterial->cullMode = CullMode::None;
        skyboxMaterial->enableDepthTest = false;
        skyboxMaterial->enableDepthWrite = false;
        skyboxMaterial->name = "skybox";

        Entity entity = mScene.GetEntityByName("CubeCube");
        entity.GetComponent<MeshRendererComponent>().material = MaterialManager::AddMaterial(skyboxMaterial);

        mSun = new Light;
        mSun->Initialize();
        mSun->SetPosition(glm::vec3(20.f));
        mSun->SetDirection(glm::vec3(-1, 1, 0.18));
        mSun->SetIntensity(5);
        mSun->SetColor(glm::vec3(1.0, 0.984, 0.913));
        mSun->SetType(LightType::DirectionalLight);
        mLights.push_back(mSun);

        VkDescriptorPool descriptorPool = CreateDescriptorPool({
                                                                   {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE},
                                                                   {VK_DESCRIPTOR_TYPE_SAMPLER, IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE},
                                                               },
                                                               IMGUI_IMPL_VULKAN_MINIMUM_SAMPLED_IMAGE_POOL_SIZE + IMGUI_IMPL_VULKAN_MINIMUM_SAMPLER_POOL_SIZE);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForVulkan(GetWindow().GetNativeWindow(), true);
        ImGui_ImplVulkan_InitInfo initInfo =
            {
                .Instance = GraphicsContext::GetInstance(),
                .PhysicalDevice = GraphicsContext::GetPhysicalDevice(),
                .Device = GraphicsContext::GetDevice(),
                .QueueFamily = GraphicsContext::GetQueueIndices().graphics,
                .Queue = GraphicsContext::GetQueues().graphics,
                .DescriptorPool = descriptorPool,
                .MinImageCount = mSurface.swapchain.GetImageCount(),
                .ImageCount = mSurface.swapchain.GetImageCount(),
                .PipelineInfoMain =
                    {
                        .RenderPass = Renderer::GetPresentRenderPass().GetHandle(),
                        .Subpass = 0,
                        .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
                    },
            };

        ImGui_ImplVulkan_Init(&initInfo);
    }

    void OnWindowResize(const glm::uvec2 &size) override
    {
        Renderer::ResizeSurface(mSurface);
    }

    void OnKeyPress(Key key) override
    {
        if (key == Key::Escape)
        {
            Close();
        }
        if (key == Key::C)
        {
            mLights.clear();
        }
        if (key == Key::L)
        {
            Light *light = new Light;
            light->Initialize();
            light->SetPosition(mCamera.GetPosition());
            light->SetColor(glm::vec3(1.0, 0.7, 0.5));
            light->SetIntensity(5);
            light->SetType(LightType::PointLight);
            light->GenerateShadowMap(Renderer::GetRenderCommands());
            mLights.push_back(light);
        }
        if (key == Key::P)
        {
            ToggleCursor();
        }
    }

    void OnKeyRelease(Key key) override
    {
        if (key == Key::L)
        {
            mLhold = false;
        }
    }

    bool mLhold = false;

    void OnScroll(const glm::vec2 &scroll) override
    {
    }

    Light *mSun = nullptr;

    void OnUpdate() override
    {
        if (IsCursorHidden())
        {
            mController.Update();
        }
        mCamera.Calculate();

        Renderer::BeginLightPlacement();

        glm::mat4 view, projection;

        float speed = 0.1f;

        float t = GetElapsedTime() * speed;

        // mSun->SetDirection(glm::vec3(glm::sin(t), (glm::cos(t)) / 2.f, 0));

        for (int i = 0; i < mLights.size(); i++)
        {
            if (mLights[i]->GetType() == LightType::DirectionalLight)
            {

                mLights[i]->SetCamera(mCamera);
            }

            mLights[i]->GenerateShadowMap(Renderer::GetRenderCommands());
            Renderer::AddLight(*mLights[i]);
        }

        Renderer::EndLightPlacement();

        Renderer::BeginFrame(mCamera);

        for (const auto &[entity, component] : mScene.GetEntities<MeshRendererComponent>())
        {
            Renderer::Submit(component.material, component.mesh, entity.GetComponent<Transform>());
        }

        Renderer::EndFrame();

        RenderUI();

        Present();
    }

    glm::vec3 DragFloat3(std::string_view name, const glm::vec3 &initialValue, float speed = 1.f)
    {
        glm::vec3 v = initialValue;
        ImGui::DragFloat3(name.data(), &v.x, speed);
        return v;
    }

    float DragFloat(std::string_view name, float initialValue, float speed = 1.f)
    {
        float v = initialValue;
        ImGui::DragFloat(name.data(), &v, speed);
        return v;
    }

    glm::vec3 ColorEdit3(std::string_view name, const glm::vec3 &initialValue)
    {
        glm::vec3 v = initialValue;
        ImGui::ColorEdit3(name.data(), &v.x);
        return v;
    }

    Entity mSelectedEntity;

    void RenderUI()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Entities");

        for (const auto &[entity, component] : mScene.GetEntities<EntityMetadata>())
        {
            if (ImGui::Button(component.name.c_str()))
            {
                mSelectedEntity = entity;
            }
        }

        ImGui::End();

        ImGui::Begin("Properties");

        int in = (int)Renderer::GetInputInt();
        ImGui::SliderInt("renderer mode", &in, 0, 4);
        Renderer::SetInputInt(in);
        if (mSelectedEntity.IsValid())
        {
            if (mSelectedEntity.HasComponent<Transform>())
            {
                ImGui::SeparatorText("Transform");
                Transform &component = mSelectedEntity.GetComponent<Transform>();
                ImGui::DragFloat3("position", &component.position.x, 0.01f);
                ImGui::DragFloat3("rotation", &component.rotation.x, 0.01f);
                ImGui::DragFloat3("scale", &component.scale.x, 0.01f);
            }

            if (mSelectedEntity.HasComponent<MeshRendererComponent>())
            {
                MeshRendererComponent &component = mSelectedEntity.GetComponent<MeshRendererComponent>();
                ImGui::SeparatorText("Mesh Renderer");
                Mesh &cMesh = *MeshManager::GetMesh(component.mesh);
                Material &cMaterial = *MaterialManager::GetMaterial(component.material);

                if (ImGui::BeginCombo("Mesh", std::format("{} {}", (uint64_t)component.mesh, cMesh.GetName()).c_str()))
                {

                    for (const auto &[id, mesh] : MeshManager::GetMap())
                    {
                        ImGui::PushID((int)id);

                        if (ImGui::Selectable(mesh->GetName().c_str(), id == component.mesh))
                        {
                            component.mesh = id;
                        }

                        ImGui::PopID();
                    }

                    ImGui::EndCombo();
                }

                if (ImGui::BeginCombo("Material", std::format("{} {}", (uint64_t)component.material, cMaterial.name).c_str()))
                {
                    for (const auto &[id, material] : MaterialManager::GetMap())
                    {
                        ImGui::PushID((int)id);
                        if (ImGui::Selectable(material->name.c_str(), id == component.material))
                        {
                            component.material = id;
                        }
                        ImGui::PopID();
                    }

                    ImGui::EndCombo();
                }

                ImGui::SliderFloat("roughness", &cMaterial.roughnessFactor, 0.1f, 1.f);
                ImGui::SliderFloat("metallic", &cMaterial.metallicFactor, 0.01f, 1.f);
            }
        }

        mSun->SetColor(ColorEdit3("Sun color", mSun->GetColor()));
        mSun->SetIntensity(DragFloat("Sun intensity", mSun->GetIntensity(), 0.01));
        mSun->SetDirection(DragFloat3("Sun direction", mSun->GetDirection(), 0.01f));

        ImGui::End();

        ImGui::Render();
    }

    void Present()
    {
        uint32_t imageIndex = mSurface.swapchain.GetNextImageIndex(Renderer::mImageAcquiredSemaphore, {});
        if (imageIndex == UINT32_MAX)
        {
            return;
        }

        Renderer::mPresentCommandBuffer.BeginRecording();
        Renderer::mPresentRenderPass.CmdBeginRenderPass(Renderer::mPresentCommandBuffer, mSurface.frameBuffers[imageIndex], mSurface.swapchain.GetSize(), {{1, 1, 1, 1}});

        VkViewport viewport =
            {
                .width = (float)mSurface.swapchain.GetSize().x,
                .height = (float)mSurface.swapchain.GetSize().y,
                .minDepth = 0.f,
                .maxDepth = 1.f,
            };

        VkRect2D scissor =
            {
                .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
            };

        VkDescriptorSet descriptorSets[] = {Renderer::mPresentInputDescriptor.GetDescriptorSet()};
        vkCmdBindDescriptorSets(Renderer::mPresentCommandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, Renderer::mPresentPipeline.GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);
        Renderer::mPresentPipeline.CmdBindPipeline(Renderer::mPresentCommandBuffer);

        vkCmdSetViewport(Renderer::mPresentCommandBuffer.GetHandle(), 0, 1, &viewport);
        vkCmdSetScissor(Renderer::mPresentCommandBuffer.GetHandle(), 0, 1, &scissor);
        vkCmdSetCullMode(Renderer::mPresentCommandBuffer.GetHandle(), VK_CULL_MODE_NONE);
        vkCmdSetDepthTestEnable(Renderer::mPresentCommandBuffer.GetHandle(), false);
        vkCmdSetDepthWriteEnable(Renderer::mPresentCommandBuffer.GetHandle(), false);

        vkCmdDraw(Renderer::mPresentCommandBuffer.GetHandle(), 6, 1, 0, 0);

        if (!IsCursorHidden())
        {
            ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), Renderer::mPresentCommandBuffer.GetHandle());
        }

        Renderer::mPresentRenderPass.CmdEndRenderPass(Renderer::mPresentCommandBuffer);
        Renderer::mPresentCommandBuffer.EndRecording();

        Renderer::mPresentCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics, Renderer::mImageAcquiredSemaphore, Renderer::mSwapchainRenderFinished, PipelineStage::ColorAttachmentOutput);

        VkSwapchainKHR swapchain[] = {mSurface.swapchain.GetHandle()};
        VkSemaphore waitSemaphores[] = {Renderer::mSwapchainRenderFinished.GetHandle()};

        VkPresentInfoKHR presentInfo =
            {
                .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
                .waitSemaphoreCount = 1,
                .pWaitSemaphores = waitSemaphores,
                .swapchainCount = 1,
                .pSwapchains = swapchain,
                .pImageIndices = &imageIndex,
            };

        vkQueuePresentKHR(GraphicsContext::GetQueues().graphics, &presentInfo);

        vkDeviceWaitIdle(GraphicsContext::GetDevice());
    }

    void OnEnd() override
    {
    }
};

CREATE_APPLICATION(RendererRework)
