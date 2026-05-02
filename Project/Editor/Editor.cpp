#include "Editor.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"
#include "CameraController.hpp"

void Editor::OnStart()
{
    mEditorLayer.reset(new EditorLayer);
    AttachLayer(mEditorLayer);


    GetWindowRef().SetFullscreen(true);
    
    mCameraController.SetCamera(mCamera, GetWindowRef());

    Vertex vertices[] = 
    {
        // Front
        {{ -0.5, -0.5,  0.5 }, { 0, 0 }, { 0,  0,  1 }},
        {{  0.5, -0.5,  0.5 }, { 1, 0 }, { 0,  0,  1 }},
        {{  0.5,  0.5,  0.5 }, { 1, 1 }, { 0,  0,  1 }},
        {{ -0.5,  0.5,  0.5 }, { 0, 1 }, { 0,  0,  1 }},

        // Back
        {{  0.5, -0.5, -0.5 }, { 0, 0 }, { 0,  0, -1 }},
        {{ -0.5, -0.5, -0.5 }, { 1, 0 }, { 0,  0, -1 }},
        {{ -0.5,  0.5, -0.5 }, { 1, 1 }, { 0,  0, -1 }},
        {{  0.5,  0.5, -0.5 }, { 0, 1 }, { 0,  0, -1 }},

        // Left
        {{ -0.5, -0.5, -0.5 }, { 0, 0 }, { -1, 0,  0 }},
        {{ -0.5, -0.5,  0.5 }, { 1, 0 }, { -1, 0,  0 }},
        {{ -0.5,  0.5,  0.5 }, { 1, 1 }, { -1, 0,  0 }},
        {{ -0.5,  0.5, -0.5 }, { 0, 1 }, { -1, 0,  0 }},

        // Right
        {{  0.5, -0.5,  0.5 }, { 0, 0 }, {  1, 0,  0 }},
        {{  0.5, -0.5, -0.5 }, { 1, 0 }, {  1, 0,  0 }},
        {{  0.5,  0.5, -0.5 }, { 1, 1 }, {  1, 0,  0 }},
        {{  0.5,  0.5,  0.5 }, { 0, 1 }, {  1, 0,  0 }},

        // Top 
        {{ -0.5,  0.5,  0.5 }, { 0, 0 }, {  0, 1,  0 }},
        {{  0.5,  0.5,  0.5 }, { 1, 0 }, {  0, 1,  0 }},
        {{  0.5,  0.5, -0.5 }, { 1, 1 }, {  0, 1,  0 }},
        {{ -0.5,  0.5, -0.5 }, { 0, 1 }, {  0, 1,  0 }},

        // Bottom
        {{ -0.5, -0.5, -0.5 }, { 0, 0 }, {  0, -1, 0 }},
        {{  0.5, -0.5, -0.5 }, { 1, 0 }, {  0, -1, 0 }},
        {{  0.5, -0.5,  0.5 }, { 1, 1 }, {  0, -1, 0 }},
        {{ -0.5, -0.5,  0.5 }, { 0, 1 }, {  0, -1, 0 }},
    };

    uint32_t indices[] = 
    {
         2,  1,  0,    3,  2,  0,  // Front
         6,  5,  4,    7,  6,  4,  // Back
        10,  9,  8,   11, 10,  8, // Left
        14, 13, 12,   15, 14, 12, // Right
        18, 17, 16,   19, 18, 16, // Top
        22, 21, 20,   23, 22, 20, // Bottom
    };

    mesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));

    skyboxMaterial.LoadShaders("Shaders/skybox.vert.spv", "Shaders/skybox.frag.spv");
    skyboxMaterial.SetCullMode(CullMode::None);
    skyboxMaterial.EnableDepthTestEnable(false);
    skyboxMaterial.EnableDepthWriteEnable(false);
    skyboxMaterial.Create();

    material.LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
    material.LoadAlbedo("Textures/sample.png");
    material.SetCullMode(CullMode::Front);
    material.Create();

    mTarget.Create(GetWindowRef().GetFrameBufferSize());
    mTarget.TransitionLayout(ImageLayout::General);

    InitializeImgui();
}

void Editor::UpdateCamera()
{
    mCameraController.Update();
    mCamera.Calculate();
}

void Editor::OnUpdate()
{
    UpdateCamera();

    GetRendererRef().BeginFrame(mTarget, mCamera);

    GetRendererRef().Submit(mesh, skyboxMaterial);
    GetRendererRef().Submit(mesh, material);

    GetRendererRef().EndFrame();

    RenderImgui();
}

void Editor::OnEnd()
{
    
}

void Editor::OnKeyPress(Key key) 
{
    if (key == Key::Escape) 
        Close();
    if(key == Key::F11)
    {
        GetWindowRef().SetFullscreen(!GetWindowRef().isFullscreen());
    }
}

void Editor::OnWindowResize(const glm::uvec2 &size) 
{
    GetRendererRef().ResizeSwapchain(size);

    for (int i = 0; i < GetRendererRef().mSwapchain.GetImageCount(); i++)
    {
        mImguiFrameBuffer[i].Destroy();
        FrameBuffer frameBuffer;
        frameBuffer.Create(GetRendererRef().mSwapchain.GetSize(), {GetRendererRef().mSwapchain.GetImages()[i]}, mImguiRenderPass);
        mImguiFrameBuffer[i] = frameBuffer;
    }
}

void Editor::OnMouseMove(const glm::vec2 &position, const glm::vec2 &offset) 
{
}

void CustomStyle() 
{
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./inter.ttf", 18);

    ImGuiStyle &style = ImGui::GetStyle(); 
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.01,0.01,0.01,1);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.02,0.02,0.02,1);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.01,0.01,0.01,1);
    style.Colors[ImGuiCol_Tab] = ImVec4(0,0,0,1);
 
}

void Editor::InitializeImgui() 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

    mImguiRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mImguiRenderPass.AddSubpass({0}, {}, UINT32_MAX, PipelineBindPoint::Graphic);
    mImguiRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mImguiRenderPass.Create();
    
    ImGui_ImplVulkan_InitInfo initInfo = 
    {
        .Instance = getInstance(),
        .PhysicalDevice = getPhysicalDevice(),
        .Device = getDevice(),
        .QueueFamily = getQueueIndices().graphics,
        .Queue = getQueues().graphics,
        .DescriptorPool = descriptorPool,
        .MinImageCount = GetRendererRef().mSwapchain.GetImageCount(),
        .ImageCount = GetRendererRef().mSwapchain.GetImageCount(),
        .PipelineInfoMain = 
        {
            .RenderPass = mImguiRenderPass.GetHandle(),
            .Subpass = 0,
            .SwapChainImageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        },
    };

    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);



    for (int i = 0; i < GetRendererRef().mSwapchain.GetImageCount(); i++)
    {
        FrameBuffer frameBuffer;
        frameBuffer.Create(GetRendererRef().mSwapchain.GetSize(), {GetRendererRef().mSwapchain.GetImages()[i]}, mImguiRenderPass);
        mImguiFrameBuffer.push_back(frameBuffer);
    }

    mImGuiCommandBuffer.Create();
    mImageAcquiredSemaphore.Create();
    mRenderingFinished.Create();

    mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRendererRef().mDefaultSampler.GetHandle(), mTarget.GetImage().view, VK_IMAGE_LAYOUT_GENERAL);

    mImage = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRendererRef().mDefaultSampler.GetHandle(), GetRendererRef().mDeferredAttachments.albedo.view, VK_IMAGE_LAYOUT_UNDEFINED);
}

void WriteColorStyleFromFile()
{

}

void Editor::ResizeRenderView(const glm::uvec2& size)
{
    if(mTarget.GetImage().size != size)
    {
        vkDeviceWaitIdle(getDevice());
        mTarget.Resize(size);
        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)mRenderViewTexture);
        mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRendererRef().mDefaultSampler.GetHandle(), mTarget.GetImage().view, VK_IMAGE_LAYOUT_GENERAL);
    }
}

void Editor::RenderUi()
{
    ImGui::Begin("Game View");
    
    mCameraController.EnableControl(false);

    if(ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        GetWindowRef().HideCursor();
        mCameraController.EnableControl(true);
    }
    else if(ImGui::IsWindowHovered() && !ImGui::IsMouseDown(ImGuiMouseButton_Left))
    {
        GetWindowRef().ShowCursor();
    }


    mViewSize.x = ImGui::GetContentRegionAvail().x;
    mViewSize.y = ImGui::GetContentRegionAvail().y;

    ResizeRenderView(mViewSize);

    mCamera.SetAspectRatio((float)mViewSize.x / (float)mViewSize.y);
    ImGui::Image(mRenderViewTexture, {(float)mTarget.GetImage().size.x, (float)mTarget.GetImage().size.y}, ImVec2(0,1), ImVec2(1,0));

    ImGui::End();

    ImGui::Begin("Control Panel");
    glm::vec3 position = mCamera.GetPosition();
    ImGui::DragFloat3("Position", &position.x, 0.1f);
    mCamera.SetPosition(position);

    ImGui::End();

    if(mEnableImageViewer)
    {
        static int currentOption = 0;
        const char* imageViewerOptions[] = {"G Buffer Albedo", "G Buffer Position", "G Buffer Normal", "G Buffer Depth"};
        ImGui::Begin("Image Viewer");
        if(ImGui::Combo("Images", &currentOption, imageViewerOptions, 4))
        {
            VkImageView imageView = GetRendererRef().mDeferredAttachments.albedo.view;

            switch (currentOption) 
            {
                case 0:
                    imageView = GetRendererRef().mDeferredAttachments.albedo.view;
                    break;
                case 1:
                    imageView = GetRendererRef().mDeferredAttachments.position.view;
                    break;
                case 2:
                    imageView = GetRendererRef().mDeferredAttachments.normal.view;
                    break;
                case 3:
                    imageView = GetRendererRef().mDeferredAttachments.depth.view;
                    break;
            }

            VkDescriptorImageInfo descriptorImage[1] = {};
            descriptorImage[0].sampler = GetRendererRef().mDefaultSampler.GetHandle();
            descriptorImage[0].imageView = imageView;
            descriptorImage[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            VkWriteDescriptorSet writeDescriptor[1] = {};
            writeDescriptor[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeDescriptor[0].dstSet = (VkDescriptorSet)mImage;
            writeDescriptor[0].descriptorCount = 1;
            writeDescriptor[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeDescriptor[0].pImageInfo = descriptorImage;
            vkUpdateDescriptorSets(getDevice(), 1, writeDescriptor, 0, nullptr);
        }

        ImGui::Image(mImage, ImGui::GetContentRegionAvail(), ImVec2(0,1), ImVec2(1,0));
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
            ImGui::Checkbox("Image View", &mEnableImageViewer);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::RenderImgui() 
{
    ImGui_ImplVulkan_NewFrame();    
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::DockSpaceOverViewport();

    RenderUi();

    ImGui::Render();

    uint32_t imageIndex = GetRendererRef().mSwapchain.GetNextImageIndex(mImageAcquiredSemaphore, {});

    mImGuiCommandBuffer.BeginRecording();

    mImguiRenderPass.CmdBeginRenderPass(mImGuiCommandBuffer, mImguiFrameBuffer[imageIndex], GetWindowRef().GetSize(), {{0,0,0,1}});

    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), mImGuiCommandBuffer.GetHandle());

    mImguiRenderPass.CmdEndRenderPass(mImGuiCommandBuffer);

    mImGuiCommandBuffer.EndRecording();

    mImGuiCommandBuffer.QueueSubmit(getQueues().graphics, mImageAcquiredSemaphore, mRenderingFinished);

    VkSemaphore waitSemaphores[] = {mRenderingFinished.GetHandle()};
    VkSwapchainKHR swapchains[] = {GetRendererRef().mSwapchain.GetHandle()};

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
}

void EditorLayer::OnAttach() 
{
    LOG("On Attach");    
}

void EditorLayer::OnUpdate() 
{
    LOG("On Update");    
}

void EditorLayer::OnDetach() 
{
    LOG("On Detach");    
}

CREATE_APPLICATION(Editor);