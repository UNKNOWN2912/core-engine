#include "Editor.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

void Editor::OnStart()
{
    GetWindowRef().SetFullscreen(true);

    Vertex vertices[] = 
    {
        {glm::vec3( 0.5, 0.5, 0.0), glm::vec2(0), glm::vec3(0,0,0)},
        {glm::vec3( 0.5,-0.5, 0.0), glm::vec2(0), glm::vec3(0,0,0)},
        {glm::vec3(-0.5,-0.5, 0.0), glm::vec2(0), glm::vec3(0,0,0)},
    };

    uint32_t indices[] = 
    {
        0,1,2,
    };

    mesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));

    material.LoadShaders("Shaders/shader.vert.spv", "Shaders/shader.frag.spv");
    material.SetCullMode(CullMode::None);
    material.Create();

    renderCommand.pipeline = material.GetPipeline();
    renderCommand.vertexBuffer = mesh.GetVertexBuffer();
    renderCommand.indexBuffer = mesh.GetIndexBuffer();

    mTarget.Create(GetWindowRef().GetFrameBufferSize());
    mTarget.TransitionLayout(ImageLayout::General);

    InitializeImgui();
    
}

void Editor::OnUpdate()
{
    GetRendererRef().BeginFrame(mTarget);

    GetRendererRef().Submit(renderCommand);

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
}

void Editor::OnWindowResize(const glm::uvec2 &size) 
{
    // mTarget.Resize(size);
    // ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)mRenderViewTexture);
    // mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRendererRef().mDefaultSampler.GetHandle(), mTarget.GetImage().view, VK_IMAGE_LAYOUT_GENERAL);

    GetRendererRef().ResizeSwapchain(size);

    
    for (int i = 0; i < GetRendererRef().mSwapchain.GetImageCount(); i++)
    {
        mImguiFrameBuffer[i].Destroy();
        FrameBuffer frameBuffer;
        frameBuffer.Create(GetRendererRef().mSwapchain.GetSize(), {GetRendererRef().mSwapchain.GetImages()[i]}, mImguiRenderPass);
        mImguiFrameBuffer[i] = frameBuffer;
    }
}

void CustomStyle() 
{
    ImGui::GetIO().Fonts->AddFontFromFileTTF("./inter.ttf");

    ImGuiStyle &style = ImGui::GetStyle(); 
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.01,0.01,0.01,1);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.02,0.02,0.02,1);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.01,0.01,0.01,1);

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
}



void Editor::RenderUi()
{
    ImGui::Begin("Game View");

    mViewSize.x = ImGui::GetContentRegionAvail().x;
    mViewSize.y = ImGui::GetContentRegionAvail().y;

    if(mTarget.GetImage().size != mViewSize)
    {
        vkDeviceWaitIdle(getDevice());
        mTarget.Resize(mViewSize);
        ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)mRenderViewTexture);
        mRenderViewTexture = (ImTextureID)ImGui_ImplVulkan_AddTexture(GetRendererRef().mDefaultSampler.GetHandle(), mTarget.GetImage().view, VK_IMAGE_LAYOUT_GENERAL);
    }

    ImGui::Image(mRenderViewTexture, {(float)mTarget.GetImage().size.x, (float)mTarget.GetImage().size.y});

    ImGui::End();

    ImGui::Begin("Control Panel");



    ImGui::End();
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


CREATE_APPLICATION(Editor);