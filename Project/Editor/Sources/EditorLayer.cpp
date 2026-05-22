#include "EditorLayer.hpp"
#include "Core/Application.hpp"
#include "EditorPanels.hpp"
#include "GameLayer.hpp"
#include "ImGuiHelper.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include <string>

void EditorLayer::OnAttach()
{
    mEditorCameraController.SetCamera(mEditorCamera, Application::GetInstance()->GetWindowRef());
    InitializeImGui();

    ImGuiStyle &style = ImGui::GetStyle();

    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        const float threshold = 0.04045f;
        const float divisor = 12.92f;
        const float offset = 0.055f;
        const float divisor2 = 1.055f;
        const float power = 2.4f;

        ImVec4 &col = style.Colors[i];
        col.x = col.x <= threshold ? col.x / divisor
                                   : glm::pow((col.x + offset) / divisor2, power);
        col.y = col.y <= threshold ? col.y / divisor
                                   : glm::pow((col.y + offset) / divisor2, power);
        col.z = col.z <= threshold ? col.z / divisor
                                   : glm::pow((col.z + offset) / divisor2, power);
    }

    mScene = &GetLayer<GameLayer>().scene;
    mRenderTarget = &GetLayer<GameLayer>().mTarget;

    mPanelManager.AddPanel<GameViewPanel>(mRenderTarget, &mEditorCamera, &mEditorCameraController);
    mPanelManager.AddPanel<EntityPanel>();
    mPanelManager.AddPanel<PropertyPanel>();
    mPanelManager.AddPanel<PerformancePanel>();
    mPanelManager.AddPanel<ImageViewerPanel>();

    mPanelManager.GetPanel<EntityPanel>()->SetScene(mScene);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Albedo", &Renderer::mDeferred.attachment.albedo);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Position", &Renderer::mDeferred.attachment.position);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Normal", &Renderer::mDeferred.attachment.normal);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Depth", &Renderer::mDeferred.attachment.depth);

    for (const auto &[id, texture] : TextureManager::GetMap())
    {
        mPanelManager.GetPanel<ImageViewerPanel>()->AddImage(TextureManager::GetTexture(id)->GetName(), &texture->GetImageRef());
    }
}

void EditorLayer::UpdateCamera()
{
    mEditorCameraController.Update();
    mEditorCamera.Calculate();
}

void EditorLayer::OnUpdate()
{
    mPanelManager.InvokeOnUpdate();
    mPanelManager.GetPanel<PropertyPanel>()->SetEntity(mPanelManager.GetPanel<EntityPanel>()->GetSelectedEntity());

    UpdateCamera();
    RenderImGui();
}

void EditorLayer::OnDetach()
{
    TerminateImGui();
}

void EditorLayer::RenderUI()
{
    mPanelManager.InvokeOnRenderUi();
    MainMenuBar();
}

namespace
{
void iconWindowToggle(std::string_view label, char iconCharacter, bool &opened)
{
    ImGuiHelper::IconCharacterSameLine(iconCharacter);
    if (ImGui::MenuItem(label.data()))
    {
        opened = !opened;
    }
}
}; // namespace

void EditorLayer::FileMenu()
{
    if (!ImGui::BeginMenu("File"))
    {
        return;
    }

    ImGui::EndMenu();
}

void EditorLayer::EditMenu()
{
    if (!ImGui::BeginMenu("Edit"))
    {
        return;
    }

    ImGuiHelper::IconCharacter('W');
    ImGui::SameLine();
    if (ImGui::MenuItem("Reload icon font"))
    {
        ImGui::GetIO().Fonts->RemoveFont(ImGuiHelper::iconFont);
        ImGuiHelper::iconFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("./Fonts/icon.ttf");
    }
    ImGuiHelper::IconCharacter('W');
    ImGui::SameLine();
    if (ImGui::MenuItem("Reload material"))
    {
        GetLayer<GameLayer>().ReloadMaterial();
        Renderer::mLighting.pipeline.Destroy();
        Renderer::mLighting.pipeline.LoadShader("Shaders/swapchain.comp.spv");
        Renderer::mLighting.pipeline.Create({&Renderer::mLighting.descriptor, &Renderer::mDeferred.descriptor, &Renderer::mLighting.uniformDescriptor});
    }

    ImGui::EndMenu();
}

void EditorLayer::ViewMenu()
{
    if (!ImGui::BeginMenu("View"))
    {
        return;
    }

    for (const auto &[id, panel] : mPanelManager.GetPanelMap())
    {
        if (ImGuiHelper::IconMenuItem(panel->GetTitle(), panel->GetIcon()))
        {
        }
    }

    ImGui::EndMenu();
}

void EditorLayer::WindowMenu()
{

    if (!ImGui::BeginMenu("Window"))
    {
        return;
    }

    // ImGuiHelper::IconCharacterSameLine('A');
    if (ImGuiHelper::IconMenuItem("Close", 'A'))
    {
        Application::GetInstance()->Close();
    }

    ImGuiHelper::IconCharacterSameLine(GetWindow().IsFullscreen() ? '?' : '>');
    if (ImGui::MenuItem("Toggle Fullscreen"))
    {
        GetWindow().SetFullscreen(!GetWindow().IsFullscreen());
    }

    if (GetWindow().IsMaximized())
    {
        ImGuiHelper::IconCharacterSameLine('m');
        if (ImGui::MenuItem("Minimize"))
        {
            GetWindow().Restore();
        }
    }
    else
    {
        ImGuiHelper::IconCharacterSameLine('n');
        if (ImGui::MenuItem("Maximize"))
        {
            GetWindow().Maximize();
        }
    }

    ImGui::EndMenu();
}

void EditorLayer::MainMenuBar()
{
    if (!ImGui::BeginMainMenuBar())
    {
        return;
    }

    FileMenu();
    EditMenu();
    ViewMenu();
    WindowMenu();

    ImGui::EndMainMenuBar();
}

void EditorLayer::SetRenderTarget(RenderTarget *renderTarget)
{
    mRenderTarget = renderTarget;
}
const Camera &EditorLayer::GetEditorCamera() const
{
    return mEditorCamera;
}

bool EditorLayer::OnEvent(uint32_t code, void *data)
{
    WindowEvent windowEvent = (WindowEvent)code;

    if (windowEvent == WindowEvent::WindowResize)
    {
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            vkDeviceWaitIdle(GraphicsContext::GetDevice());
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i] = FrameBuffer(Renderer::GetSwapchain().GetSize(),
                                               {Renderer::GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
    return false;
}

void EditorLayer::CustomStyle()
{
    const float fontSize = 18.f;
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./Fonts/inter.ttf", fontSize);
    ImGuiHelper::iconFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("./Fonts/icon.ttf", 16);
}

void EditorLayer::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    CustomStyle();

    VkDescriptorPoolSize poolSize =
        {
            .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
        };

    VkDescriptorPoolCreateInfo createInfo =
        {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
            .maxSets = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
            .poolSizeCount = 1,
            .pPoolSizes = &poolSize,
        };

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    vkCreateDescriptorPool(GraphicsContext::GetDevice(), &createInfo, nullptr, &descriptorPool);

    mImGuiRenderPass.AddAttachment(Renderer::GetSwapchain().GetFormat(), ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mImGuiRenderPass.AddSubpass({0}, {}, UINT32_MAX, PipelineBindPoint::Graphic);
    mImGuiRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mImGuiRenderPass.CreateRenderPass();

    ImGui_ImplVulkan_InitInfo initInfo =
        {
            .Instance = GraphicsContext::GetInstance(),
            .PhysicalDevice = GraphicsContext::GetPhysicalDevice(),
            .Device = GraphicsContext::GetDevice(),
            .QueueFamily = GraphicsContext::GetQueueIndices().graphics,
            .Queue = GraphicsContext::GetQueues().graphics,
            .DescriptorPool = descriptorPool,
            .MinImageCount = Renderer::GetSwapchain().GetImageCount(),
            .ImageCount = Renderer::GetSwapchain().GetImageCount(),
            .PipelineInfoMain =
                {
                    .RenderPass = mImGuiRenderPass.GetHandle(),
                    .Subpass = 0,
                    .SwapChainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                },
        };

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow *)Application::GetInstance()->GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    mImGuiFrameBuffer.reserve(Renderer::GetSwapchain().GetImageCount());
    for (int i = 0; i < Renderer::GetSwapchain().GetImageCount(); i++)
    {
        mImGuiFrameBuffer.emplace_back(Renderer::GetSwapchain().GetSize(), std::initializer_list<Image>{Renderer::GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
    }

    mImGuiCommandBuffer.Create();
    mImageAcquiredSemaphore.Create();
    mRenderingFinished.Create();
}

void EditorLayer::TerminateImGui()
{
    vkDeviceWaitIdle(GraphicsContext::GetDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void EditorLayer::RenderImGui()
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    RenderUI();

    ImGui::Render();

    uint32_t imageIndex = Renderer::GetSwapchain().GetNextImageIndex(mImageAcquiredSemaphore, {});

    mImGuiCommandBuffer.BeginRecording();

    mImGuiRenderPass.CmdBeginRenderPass(mImGuiCommandBuffer, mImGuiFrameBuffer[imageIndex], Renderer::GetSwapchain().GetSize(), {{0, 0, 0, 1}});

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuiCommandBuffer.GetHandle());

    mImGuiRenderPass.CmdEndRenderPass(mImGuiCommandBuffer);

    mImGuiCommandBuffer.EndRecording();

    mImGuiCommandBuffer.QueueSubmit(GraphicsContext::GetQueues().graphics, mImageAcquiredSemaphore, mRenderingFinished);

    std::array<VkSemaphore, 2> waitSemaphores = {mRenderingFinished.GetHandle()};
    std::array<VkSwapchainKHR, 2> swapchains = {Renderer::GetSwapchain().GetHandle()};

    VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores.data(),
            .swapchainCount = 1,
            .pSwapchains = swapchains.data(),
            .pImageIndices = &imageIndex,
        };

    vkQueuePresentKHR(GraphicsContext::GetQueues().present, &presentInfo);

    if (Renderer::ResizeSwapchain(Application::GetInstance()->GetWindowRef().GetSize()))
    {
        vkDeviceWaitIdle(GraphicsContext::GetDevice());
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i] = FrameBuffer(Renderer::GetSwapchain().GetSize(),
                                               {Renderer::GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
}
