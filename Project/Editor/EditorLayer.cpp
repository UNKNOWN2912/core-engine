#include "EditorLayer.hpp"
#include "Core/Application.hpp"
#include "GameLayer.hpp"
#include "ImGuiHelper.hpp"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

void EditorLayer::OnAttach() 
{
    SetRenderTarget(&GetLayer<GameLayer>().mTarget);

    mRenderer = &Application::GetInstance()->GetRendererRef();
    InitializeImGui();

    mEditorCameraController.SetCamera(mEditorCamera, Application::GetInstance()->GetWindowRef());

    SetImage("Albedo", mRenderer->GetDeferredAttachments().albedo);
    SetImage("Position", mRenderer->GetDeferredAttachments().position);
    SetImage("Normal", mRenderer->GetDeferredAttachments().normal);
    SetImage("Depth", mRenderer->GetDeferredAttachments().depth);

    mRenderer->AddListener(BindMember(EditorLayer::RendererEventCallback));
    mImageViewId = (ImTextureID)ImGui_ImplVulkan_AddTexture(mRenderer->GetDefaultSampler().GetHandle(), mRenderer->GetDeferredAttachments().albedo.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void EditorLayer::UpdateCamera()
{
    mEditorCameraController.Update();
    mEditorCamera.Calculate();
}

void EditorLayer::OnUpdate() 
{
    UpdateCamera();
    RenderImGui();
    ResizeRenderView(mViewSize);
}

void EditorLayer::OnDetach() 
{
    TerminateImGui();
}

void EditorLayer::SetImageView(const Image& image)
{
    VkDescriptorImageInfo imageInfo[1] = {};
    imageInfo[0].sampler = mRenderer->GetDefaultSampler().GetHandle();
    imageInfo[0].imageView = image.view;
    imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    VkWriteDescriptorSet writeDescriptor[1] = {};
    writeDescriptor[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptor[0].dstSet = (VkDescriptorSet)mImageViewId;
    writeDescriptor[0].descriptorCount = 1;
    writeDescriptor[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptor[0].pImageInfo = imageInfo;
    vkUpdateDescriptorSets(getDevice(), 1, writeDescriptor, 0, nullptr);
}

void EditorLayer::RenderUI() 
{
    ImGui::Begin("Game View");
    
    mViewSize = glm::uvec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
    
    ImGui::Image(mRenderViewTexture, ImVec2(mViewSize.x, mViewSize.y), ImVec2(0,1), ImVec2(1,0));
    mEditorCameraController.EnableKeyboardControl(ImGui::IsItemHovered());
    mEditorCameraController.EnableMouseControl(ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left));

    ImGui::End();

    ImGui::Begin("Control Panel");

    ImGui::SeparatorText("Camera");

    ImGuiHelper::DragVec3("Position", mEditorCamera.GetPositionRef(), 0.01f);
    ImGuiHelper::DragVec3("Front", mEditorCamera.GetFrontRef(), 0.01f);
    ImGuiHelper::DragVec3("Up", mEditorCamera.GetUpRef(), 0.01f);
    ImGui::DragFloat("Fov", &mEditorCamera.GetFovRef(), 0.1f);
    ImGui::DragFloat("Aspect Ratio", &mEditorCamera.GetAspectRatioRef(), 0.1f);
    ImGui::DragFloat("Near Plane", &mEditorCamera.GetNearPlaneRef(), 0.1f);
    ImGui::DragFloat("Far Plane", &mEditorCamera.GetFarPlaneRef(), 0.1f);

    ImGui::SeparatorText("Camera Controller");

    ImGui::DragFloat("Speed", &mEditorCameraController.GetSpeedRef());
    ImGui::DragFloat("Sensitivity", &mEditorCameraController.GetSensitivityRef());

    ImGui::End();

    if(mEnableImageViewer)
    {
        ImGui::Begin("Image Viewer");
        
        static std::string currentItem = "";

        if(ImGui::BeginCombo("Images", currentItem.c_str()))
        {
            for (auto& [name, image] : mImageMap) 
            {
                if(ImGui::Selectable(name.c_str(), currentItem == name))
                {
                    currentItem = name;

                    SetImageView(mImageMap[currentItem]);
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Image(mImageViewId, ImGui::GetContentRegionAvail(), {0,1}, {1,0});
        ImGui::End();
    }

    if(ImGui::BeginMainMenuBar())
    {
        if(ImGui::BeginMenu("File"))
        {
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Edit"))
        {
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("View"))
        {
            ImGui::Checkbox("Image Viewer", &mEnableImageViewer);
            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Window"))
        {
            if (ImGui::MenuItem("Close"))
            {
                Application::GetInstance()->Close();
            }
            if(ImGui::MenuItem("Toggle Fullscreen"))
            {
                Application::GetInstance()->GetWindowRef().SetFullscreen(!Application::GetInstance()->GetWindowRef().isFullscreen());
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void EditorLayer::SetImage(const std::string& name, const Image& image) 
{
    mImageMap[name] = image;    
}

bool EditorLayer::RendererEventCallback(uint32_t code, void* data) 
{
    RendererEvent event = (RendererEvent)code;

    switch (event) 
    {
        case RendererEvent::DeferredAttachmentResize:
        {
            vkDeviceWaitIdle(getDevice());
            DeferredSubpassAttachment* attachment = (DeferredSubpassAttachment*)data;
            SetImage("Albedo", attachment->albedo);
            SetImage("Position", attachment->position);
            SetImage("Normal", attachment->normal);
            SetImage("Depth", attachment->depth);
        }
        break;
    }

    return false;
}

bool EditorLayer::OnEvent(uint32_t code, void *data) 
{
    WindowEvent windowEvent = (WindowEvent)code;

    if(windowEvent == WindowEvent::WindowResize)
    {
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(mRenderer->GetSwapchain().GetSize(), 
            {mRenderer->GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }

    return false;
}


void customStyle() 
{
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./inter.ttf", 18);

    ImGuiStyle &style = ImGui::GetStyle(); 
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.01,0.01,0.01,1);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.02,0.02,0.02,1);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.01,0.01,0.01,1);
    style.Colors[ImGuiCol_Tab] = ImVec4(0,0,0,1);
 
}

void EditorLayer::InitializeImGui() 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    customStyle();

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

    mImGuiRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mImGuiRenderPass.AddSubpass({0}, {}, UINT32_MAX, PipelineBindPoint::Graphic);
    mImGuiRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mImGuiRenderPass.Create();
    
    ImGui_ImplVulkan_InitInfo initInfo = 
    {
        .Instance = getInstance(),
        .PhysicalDevice = getPhysicalDevice(),
        .Device = getDevice(),
        .QueueFamily = getQueueIndices().graphics,
        .Queue = getQueues().graphics,
        .DescriptorPool = descriptorPool,
        .MinImageCount = mRenderer->GetSwapchain().GetImageCount(),
        .ImageCount = mRenderer->GetSwapchain().GetImageCount(),
        .PipelineInfoMain = 
        {
            .RenderPass = mImGuiRenderPass.GetHandle(),
            .Subpass = 0,
            .SwapChainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        },
    };

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)Application::GetInstance()->GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    for (int i = 0; i < mRenderer->GetSwapchain().GetImageCount(); i++)
    {
        FrameBuffer frameBuffer;
        frameBuffer.Create(mRenderer->GetSwapchain().GetSize(), {mRenderer->GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        mImGuiFrameBuffer.push_back(frameBuffer);
    }

    mImGuiCommandBuffer.Create();
    mImageAcquiredSemaphore.Create();
    mRenderingFinished.Create();

    if(mRenderTarget != nullptr)
        mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(mRenderer->GetDefaultSampler().GetHandle(), mRenderTarget->GetImage().view, VK_IMAGE_LAYOUT_GENERAL);
    

    mImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(mRenderer->GetDefaultSampler().GetHandle(), mRenderer->GetDeferredAttachments().albedo.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void EditorLayer::TerminateImGui() 
{
    
}

void EditorLayer::RenderImGui() 
{
    ImGui_ImplVulkan_NewFrame();    
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    RenderUI();

    ImGui::Render();

    uint32_t imageIndex = mRenderer->GetSwapchain().GetNextImageIndex(mImageAcquiredSemaphore, {});

    mImGuiCommandBuffer.BeginRecording();

    mImGuiRenderPass.CmdBeginRenderPass(mImGuiCommandBuffer, mImGuiFrameBuffer[imageIndex], Application::GetInstance()->GetRendererRef().GetSwapchain().GetSize(), {{0,0,0,1}});

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuiCommandBuffer.GetHandle());

    mImGuiRenderPass.CmdEndRenderPass(mImGuiCommandBuffer);

    mImGuiCommandBuffer.EndRecording();

    mImGuiCommandBuffer.QueueSubmit(getQueues().graphics, mImageAcquiredSemaphore, mRenderingFinished);

    VkSemaphore waitSemaphores[] = {mRenderingFinished.GetHandle()};
    VkSwapchainKHR swapchains[] = {mRenderer->GetSwapchain().GetHandle()};

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

    if(Application::GetInstance()->GetRendererRef().ResizeSwapchain(Application::GetInstance()->GetWindowRef().GetSize()))
    {
        vkDeviceWaitIdle(getDevice());
        for (int i = 0; i < mImGuiFrameBuffer.size(); i++)
        {
            mImGuiFrameBuffer[i].Destroy();
            mImGuiFrameBuffer[i].Create(mRenderer->GetSwapchain().GetSize(), 
            {mRenderer->GetSwapchain().GetImages()[i]}, mImGuiRenderPass);
        }
    }
    
}

void EditorLayer::ResizeRenderView(const glm::uvec2& size)
{
    if(mRenderTarget == nullptr)
        return;
    if(mRenderTarget->GetImage().size == size)
        return;

    vkDeviceWaitIdle(getDevice());

    mRenderTarget->Resize(size);
    mRenderTarget->TransitionLayout(ImageLayout::General);

    VkDescriptorImageInfo imageInfo = 
    {
        .sampler = mRenderer->GetDefaultSampler().GetHandle(), 
        .imageView = mRenderTarget->GetImage().view,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    VkWriteDescriptorSet writeDescriptor = 
    {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .dstSet = (VkDescriptorSet)mRenderViewTexture,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .pImageInfo = &imageInfo,
    };

    vkUpdateDescriptorSets(getDevice(), 1, &writeDescriptor, 0, nullptr);

    mEditorCamera.SetAspectRatio(float(size.x) / float(size.y));
}