#include "Editor.hpp"
#include "Renderer/GraphicsContext.hpp"
#include "Renderer/Utility.hpp"
#include "backends/imgui_impl_vulkan.h"
#include "imgui.h"
#include <vulkan/vulkan_core.h>


void Editor::OnStart()
{
    GetWindowRef().SetFullscreen(true);
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
         2,  1,  0,    3,  2,  0, // Front
         6,  5,  4,    7,  6,  4, // Back
        10,  9,  8,   11, 10,  8, // Left
        14, 13, 12,   15, 14, 12, // Right
        18, 17, 16,   19, 18, 16, // Top
        22, 21, 20,   23, 22, 20, // Bottom
    };

    mMesh.SetData(vertices, sizeof(vertices), indices, sizeof(indices));
    mMaterial.LoadShaders("Shaders/skybox.vert.spv", "Shaders/skybox.frag.spv");
    mMaterial.GetSettingsRef().cullMode = CullMode::None;
    mMaterial.GetSettingsRef().depthTestEnable = false;
    mMaterial.GetSettingsRef().depthWriteEnable = false;
    mMaterial.Create();

    InitializeImgui();
}

void Editor::OnUpdate()
{
    GetRendererRef().BeginFrame();
    GetRendererRef().DrawMeshWithMaterial(mMesh, mMaterial, Transform());

    StartImguiRender();

    ImGui::Begin("Window");
    mGameViewSize.x = ImGui::GetWindowSize().x;
    mGameViewSize.y = ImGui::GetWindowSize().y;

    ResizeFrameBuffer(mGameViewSize);

    ImGui::Image(mFrameBuffer, {float(GetRendererRef().mFinalImage.size.x), float(GetRendererRef().mFinalImage.size.y)});
    ImGui::End();

    EndImguiRender();

}

void Editor::ResizeFrameBuffer(const glm::uvec2& size)
{
    if(GetRendererRef().mFinalImage.size == size)
        return;

    vkDeviceWaitIdle(getDevice());

    DestroyImage(GetRendererRef().mFinalImage);
    DestroyImage(GetRendererRef().mSceneDepthAttachment);
    vkDestroyFramebuffer(getDevice(), GetRendererRef().mFinalFrameBuffer, nullptr);

    GetRendererRef().CreateFinalImageAttachment(size);
}

void Editor::OnWindowResize(const glm::uvec2 &size) 
{
    GetRendererRef().Resize(size);
    ImGui_ImplVulkan_RemoveTexture((VkDescriptorSet)mFrameBuffer.GetTexID());
    mFrameBuffer = ImGui_ImplVulkan_AddTexture(sampler, GetRendererRef().mFinalImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

}

void Editor::OnKeyPress(Key key) 
{
    if (key == Key::Escape)    
        Close();
}

void Editor::InitializeImgui() 
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    
    VkDescriptorPoolSize poolSize = 
    {
        .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
        .descriptorCount = IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE,
    };
    
    VkDescriptorPool descriptorPool = CreateDescriptorPool({poolSize}, IMGUI_IMPL_VULKAN_MINIMUM_IMAGE_SAMPLER_POOL_SIZE);
    ImGui_ImplVulkan_InitInfo initInfo = {};
    initInfo.Instance = getInstance();
    initInfo.PhysicalDevice = getPhysicalDevice();
    initInfo.Device = getDevice();
    initInfo.ImageCount = GetRendererRef().mSwapchain.GetImageCount();
    initInfo.MinImageCount = GetRendererRef().mSwapchain.GetImageCount();
    initInfo.DescriptorPool = descriptorPool;
    initInfo.Queue = getQueues().graphics;
    initInfo.QueueFamily = getQueueIndices().graphics;
    initInfo.PipelineInfoMain.RenderPass = GetRendererRef().GetMainRenderPass().GetHandle();
    initInfo.PipelineInfoMain.Subpass = 0;
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    
    ImGui_ImplGlfw_InitForVulkan((GLFWwindow*)GetWindowRef().GetNativeWindow(), true);
    ImGui_ImplVulkan_Init(&initInfo);

    VkSamplerCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
        .minLod = 1,
        .maxLod = 1,
    };

    vkCreateSampler(getDevice(), &createInfo, nullptr, &sampler);

    mFrameBuffer = ImGui_ImplVulkan_AddTexture(sampler, GetRendererRef().mFinalImage.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Editor::CustomEndFrame() 
{
    CHROME_TRACE_FUNCTION();

    GetRendererRef().mCamera.Calculate();
    GetRendererRef().UpdateUniformData();

    vkDeviceWaitIdle(getDevice());
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), GetRendererRef().mSwapchain.GetHandle(), UINT64_MAX, GetRendererRef().mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);

    GetRendererRef().mCommandBuffers.render.BeginRecording();
    
    CustomCmdMainRenderPass(imageIndex);
    
    GetRendererRef().mCommandBuffers.render.EndRecording();

    GetRendererRef().mCommandBuffers.render.QueueSubmit(getQueues().graphics, GetRendererRef().mSemaphores.imageAcquired, GetRendererRef().mSemaphores.renderingFinish, PipelineStage::ColorAttachmentOutput);

    GetRendererRef().PresentImage(getQueues().present, GetRendererRef().mSwapchain, imageIndex, GetRendererRef().mSemaphores.renderingFinish);

    GetRendererRef().mFrameRunning = false;
}

void Editor::CustomCmdMainRenderPass(uint32_t imageIndex) 
{
    VkViewport viewport = 
    {
        .width = (float)mGameViewSize.x,
        .height = (float)mGameViewSize.y,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    VkRect2D scissor = 
    {
        .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
    };

    
    GetRendererRef().mRenderPass.CmdBeginRenderPass(GetRendererRef().mCommandBuffers.render, GetRendererRef().mFinalFrameBuffer, mGameViewSize, {{1,1,1,1}, {1,0,0,1}});
    
    vkCmdSetViewport(GetRendererRef().mCommandBuffers.render.GetHandle(), 0, 1, &viewport);
    vkCmdSetScissor(GetRendererRef().mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    GetRendererRef().RenderDrawSubmitInfos(GetRendererRef().mDrawSubmitInfo);
    
    GetRendererRef().mRenderPass.CmdEndRenderPass(GetRendererRef().mCommandBuffers.render);
    

    viewport = GetRendererRef().mViewport;
    scissor.extent = {(uint32_t)viewport.width, (uint32_t)viewport.height};

    GetRendererRef().mRenderPass.CmdBeginRenderPass(GetRendererRef().mCommandBuffers.render, GetRendererRef().mSwapchainFramebuffer[imageIndex], GetRendererRef().mSwapchain.GetSize(), {{1,1,1,1}, {1,0,0,1}});
    
    vkCmdSetViewport(GetRendererRef().mCommandBuffers.render.GetHandle(), 0, 1, &GetRendererRef().mViewport);
    vkCmdSetScissor(GetRendererRef().mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), GetRendererRef().mCommandBuffers.render.GetHandle());

    
    GetRendererRef().mRenderPass.CmdEndRenderPass(GetRendererRef().mCommandBuffers.render);
}

void Editor::StartImguiRender() 
{
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Editor::EndImguiRender() 
{
    ImGui::Render();
    CustomEndFrame();
}

Application* Application::Create()
{
    return new Editor;
}