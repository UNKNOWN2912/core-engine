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
    SetRenderTarget(&GetLayer<GameLayer>().mTarget);
    mEditorCameraController.SetCamera(mEditorCamera, Application::GetInstance()->GetWindowRef());
    InitializeImGui();

    ImGuiStyle &style = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; i++)
    {
        ImVec4 &col = style.Colors[i];
        col.x = col.x <= 0.04045f ? col.x / 12.92f
                                  : pow((col.x + 0.055f) / 1.055f, 2.4f);
        col.y = col.y <= 0.04045f ? col.y / 12.92f
                                  : pow((col.y + 0.055f) / 1.055f, 2.4f);
        col.z = col.z <= 0.04045f ? col.z / 12.92f
                                  : pow((col.z + 0.055f) / 1.055f, 2.4f);
    }

    mScene = &GetLayer<GameLayer>().scene;

    mPanelManager.AddPanel<GameViewPanel>(mRenderTarget, &mEditorCamera, &mEditorCameraController);
    mPanelManager.AddPanel<EntityPanel>();
    mPanelManager.AddPanel<PropertyPanel>();
    mPanelManager.AddPanel<PerformancePanel>();
    mPanelManager.AddPanel<ImageViewerPanel>();

    mPanelManager.GetPanel<EntityPanel>()->SetScene(mScene);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Albedo", &GetRenderer().mDeferred.attachment.albedo);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Position", &GetRenderer().mDeferred.attachment.position);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Normal", &GetRenderer().mDeferred.attachment.normal);
    mPanelManager.GetPanel<ImageViewerPanel>()->AddImage("Depth", &GetRenderer().mDeferred.attachment.depth);

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

void IconWindowToggle(std::string_view label, char iconCharacter, bool &opened)
{
    ImGuiHelper::IconCharacterSameLine(iconCharacter);
    if (ImGui::MenuItem(label.data()))
    {
        opened = !opened;
    }
}

void EditorLayer::FileMenu()
{
    if (!ImGui::BeginMenu("File"))
        return;

    ImGui::EndMenu();
}

void EditorLayer::EditMenu()
{
    if (!ImGui::BeginMenu("Edit"))
        return;

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
        GetRenderer().mLighting.pipeline.Destroy();
        GetRenderer().mLighting.pipeline.LoadShader("Shaders/swapchain.comp.spv");
        GetRenderer().mLighting.pipeline.Create({&GetRenderer().mLighting.descriptor, &GetRenderer().mDeferred.descriptor, &GetRenderer().mLighting.uniformDescriptor});
    }

    ImGui::EndMenu();
}

void EditorLayer::ViewMenu()
{
    if (!ImGui::BeginMenu("View"))
        return;

    for (auto [id, panel] : mPanelManager.GetPanelMap())
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
        return;

    // ImGuiHelper::IconCharacterSameLine('A');
    if (ImGuiHelper::IconMenuItem("Close", 'A'))
        Application::GetInstance()->Close();

    ImGuiHelper::IconCharacterSameLine(GetWindow().isFullscreen() ? '?' : '>');
    if (ImGui::MenuItem("Toggle Fullscreen"))
        GetWindow().SetFullscreen(!GetWindow().isFullscreen());

    if (GetWindow().IsMaximized())
    {
        ImGuiHelper::IconCharacterSameLine('m');
        if (ImGui::MenuItem("Minimize"))
            GetWindow().Restore();
    }
    else
    {
        ImGuiHelper::IconCharacterSameLine('n');
        if (ImGui::MenuItem("Maximize"))
            GetWindow().Maximize();
    }

    ImGui::EndMenu();
}

void EditorLayer::MainMenuBar()
{
    if (!ImGui::BeginMainMenuBar())
        return;

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
            vkDeviceWaitIdle(getDevice());
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(GetRenderer().GetSwapchain().GetSize(),
                                        {GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
    return false;
}

void EditorLayer::CustomStyle()
{
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./Fonts/inter.ttf", 18);
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

    VkDescriptorPool descriptorPool;
    vkCreateDescriptorPool(getDevice(), &createInfo, nullptr, &descriptorPool);

    mImGuiRenderPass.AddAttachment(GetRenderer().GetSwapchain().GetFormat(), ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mImGuiRenderPass.AddSubpass({0}, {}, UINT32_MAX, PipelineBindPoint::Graphic);
    mImGuiRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mImGuiRenderPass.CreateRenderPass();

    ImGui_ImplVulkan_InitInfo initInfo =
        {
            .Instance = getInstance(),
            .PhysicalDevice = getPhysicalDevice(),
            .Device = getDevice(),
            .QueueFamily = getQueueIndices().graphics,
            .Queue = getQueues().graphics,
            .DescriptorPool = descriptorPool,
            .MinImageCount = GetRenderer().GetSwapchain().GetImageCount(),
            .ImageCount = GetRenderer().GetSwapchain().GetImageCount(),
            .PipelineInfoMain =
                {
                    .RenderPass = mImGuiRenderPass.GetHandle(),
                    .Subpass = 0,
                    .SwapChainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
                },
        };

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow *)Application::GetInstance()->GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    mImGuiFrameBuffer.reserve(GetRenderer().GetSwapchain().GetImageCount());
    for (int i = 0; i < GetRenderer().GetSwapchain().GetImageCount(); i++)
    {
        mImGuiFrameBuffer.emplace_back(GetRenderer().GetSwapchain().GetSize(), std::initializer_list<Image>{GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
    }

    mImGuiCommandBuffer.Create();
    mImageAcquiredSemaphore.Create();
    mRenderingFinished.Create();
}

void EditorLayer::TerminateImGui()
{
    vkDeviceWaitIdle(getDevice());
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

    uint32_t imageIndex = GetRenderer().GetSwapchain().GetNextImageIndex(mImageAcquiredSemaphore, {});

    mImGuiCommandBuffer.BeginRecording();

    mImGuiRenderPass.CmdBeginRenderPass(mImGuiCommandBuffer, mImGuiFrameBuffer[imageIndex], Application::GetInstance()->GetRendererRef().GetSwapchain().GetSize(), {{0, 0, 0, 1}});

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuiCommandBuffer.GetHandle());

    mImGuiRenderPass.CmdEndRenderPass(mImGuiCommandBuffer);

    mImGuiCommandBuffer.EndRecording();

    mImGuiCommandBuffer.QueueSubmit(getQueues().graphics, mImageAcquiredSemaphore, mRenderingFinished);

    VkSemaphore waitSemaphores[] = {mRenderingFinished.GetHandle()};
    VkSwapchainKHR swapchains[] = {GetRenderer().GetSwapchain().GetHandle()};

    VkPresentInfoKHR presentInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = waitSemaphores,
            .swapchainCount = 1,
            .pSwapchains = swapchains,
            .pImageIndices = &imageIndex,
        };

    vkQueuePresentKHR(getQueues().present, &presentInfo);

    if (Application::GetInstance()->GetRendererRef().ResizeSwapchain(Application::GetInstance()->GetWindowRef().GetSize()))
    {
        vkDeviceWaitIdle(getDevice());
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(GetRenderer().GetSwapchain().GetSize(),
                                        {GetRenderer().GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
}
