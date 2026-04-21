#include "Renderer.hpp"
#include "Core/Macro.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Transform.hpp"
#include "Utility.hpp"
#include "GraphicsContext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <print>

void Renderer::Initialize(const Window& window) 
{
    CHROME_TRACE_FUNCTION();
    mContext.Create(window, true);
    CreateSwapchain(window.GetSize());
    CreateRenderPass();
    CreateSwapchainFramebuffers();
    CreateSemaphores();
    CreateCommandBuffers();
    mUniformBuffer.Create(sizeof(UniformData), VK_SHADER_STAGE_VERTEX_BIT, 0);
}

void Renderer::Terminate() 
{
    CHROME_TRACE_FUNCTION();
}

void Renderer::DrawMeshWithMaterial(StaticMesh& mesh, Material& material, Transform transform)
{
    CHROME_TRACE_FUNCTION();
    DrawSubmitInfo submitInfo = 
    {
        .mesh = &mesh,
        .material = &material,
        .transform = transform,
    };

    mDrawSubmitInfo.push_back(submitInfo);
}

void Renderer::DrawMeshWithMaterialInstanced(StaticMesh& mesh, Material& material, InstanceBuffer& instanceBuffer, uint32_t instanceCount)
{
    CHROME_TRACE_FUNCTION();
    DrawSubmitInfo submitInfo = 
    {
        .mesh = &mesh,
        .material = &material,
        .instanceBuffer = &instanceBuffer,
        .instanced = true,
        .instanceCount = instanceCount,
    };
    
    mDrawSubmitInfo.push_back(submitInfo);
}

void Renderer::BeginFrame() 
{
    CHROME_TRACE_FUNCTION();
    mDrawSubmitInfo.clear();

    mFrameRunning = true;
}

void Renderer::EndFrame() 
{
    CHROME_TRACE_FUNCTION();

    mCamera.Calculate();
    SetUniformCameraData(mUniformData, mCamera);
    mUniformData.time = glfwGetTime();

    UpdateMaterialDescriptorSet(mDrawSubmitInfo, mUniformBuffer, mUniformData);

    vkDeviceWaitIdle(getDevice());
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), mSwapchain.handle, UINT64_MAX, mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);

    mCommandBuffers.render.BeginRecording();
    

    mRenderPass.CmdBeginRenderPass(mCommandBuffers.render.GetHandle(), mSwapchainFramebuffer[imageIndex], {mSwapchain.extent.width, mSwapchain.extent.height}, {{1,0,1,1}, {1,0,0,1}});
    
    VkRect2D scissor = 
    {
        .extent = {(uint32_t)mViewport.width, (uint32_t)mViewport.height},
    };

    vkCmdSetViewport(mCommandBuffers.render.GetHandle(), 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    RenderDrawSubmitInfos(mDrawSubmitInfo);

    mRenderPass.CmdEndRenderPass(mCommandBuffers.render.GetHandle());
    
    mCommandBuffers.render.EndRecording();

    mCommandBuffers.render.QueueSubmit(getQueues().graphics, mSemaphores.imageAcquired, mSemaphores.renderingFinish, PipelineStage::ColorAttachmentOutput);

    PresentImage(getQueues().present, mSwapchain, imageIndex, mSemaphores.renderingFinish);

    mFrameRunning = false;
}

void Renderer::Resize(const glm::uvec2& size)
{
    CHROME_TRACE_FUNCTION();
    vkDeviceWaitIdle(getDevice());

    DestroySwapchain();
    DestroySwapchainFramebuffers();
    CreateSwapchain(size);
    CreateSwapchainFramebuffers();
    mViewport.width = mSwapchain.extent.width;
    mViewport.height = mSwapchain.extent.height;
    mViewport.maxDepth = 1.f;
    mViewport.minDepth = 0.f;
}

void Renderer::CreateSwapchain(const glm::uvec2& size) 
{
    CHROME_TRACE_FUNCTION();
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getPhysicalDevice(), getSurface(), &capabilities);
    mSwapchain.extent = {size.x, size.y};

    if(mSwapchain.extent.width > capabilities.maxImageExtent.width || mSwapchain.extent.height > capabilities.maxImageExtent.height)
    {
        mSwapchain.extent = {800, 600};
    }

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), getSurface(), &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(getPhysicalDevice(), getSurface(), &formatCount, formats.data());

    for (VkSurfaceFormatKHR format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            mSwapchain.format = VK_FORMAT_B8G8R8A8_SRGB;
            mSwapchain.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        }
    }

    mSwapchain.imageCount = capabilities.minImageCount + 1 <= capabilities.maxImageCount ? capabilities.minImageCount + 1 : capabilities.minImageCount;

    VkSwapchainCreateInfoKHR createInfo = { };
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageColorSpace = mSwapchain.colorSpace;
    createInfo.imageFormat = mSwapchain.format;
    createInfo.imageExtent = mSwapchain.extent;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.minImageCount = mSwapchain.imageCount;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.surface = getSurface();

    vkCreateSwapchainKHR(getDevice(), &createInfo, nullptr, &mSwapchain.handle);

    vkGetSwapchainImagesKHR(getDevice(), mSwapchain.handle, &mSwapchain.imageCount, nullptr);
    mSwapchain.images.resize(mSwapchain.imageCount);
    vkGetSwapchainImagesKHR(getDevice(), mSwapchain.handle, &mSwapchain.imageCount, mSwapchain.images.data());

    

    for (VkImage image : mSwapchain.images)
    {
        VkImageView view = CreateImageView(image, ImageFormat::BGRA8, ImageAspect::Color);
        mSwapchain.views.push_back(view);
    } 
}

void Renderer::CreateRenderPass()
{
    CHROME_TRACE_FUNCTION();

    mRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::DepthStencil, LoadOperation::Clear, StoreOperation::DontCare);
    mRenderPass.AddSubpass({0}, {}, 1);
    mRenderPass.AddDependency(mRenderPass.ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mRenderPass.Create();
}

void Renderer::CreateSwapchainFramebuffers() 
{
    CHROME_TRACE_FUNCTION();
    VkFramebufferCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = mRenderPass.GetHandle(),
        .attachmentCount = 2,
        .width = mSwapchain.extent.width,
        .height = mSwapchain.extent.height,
        .layers = 1,
    };

    mDepthAttachment = CreateImage({mSwapchain.extent.width, mSwapchain.extent.height}, VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    for(VkImageView view : mSwapchain.views)
    {
        VkImageView attachments[] = {view, mDepthAttachment.view};
        createInfo.pAttachments = attachments;
        VkFramebuffer framebuffer;
        vkCreateFramebuffer(getDevice(), &createInfo, nullptr, &framebuffer);
        mSwapchainFramebuffer.push_back(framebuffer);
    }

    mViewport.width = mSwapchain.extent.width;
    mViewport.height = mSwapchain.extent.height;
    mViewport.maxDepth = 1.f;
    mViewport.minDepth = 0.f;

}

void Renderer::CreateSemaphores() 
{
    CHROME_TRACE_FUNCTION();
    VkSemaphoreCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    vkCreateSemaphore(getDevice(), &createInfo, nullptr, &mSemaphores.imageAcquired);
    vkCreateSemaphore(getDevice(), &createInfo, nullptr, &mSemaphores.renderingFinish);
}

void Renderer::CreateCommandBuffers() 
{
    CHROME_TRACE_FUNCTION();
    mCommandBuffers.render.Create();
}

void Renderer::DestroySwapchain() 
{
    CHROME_TRACE_FUNCTION();
    for (VkImageView view : mSwapchain.views) 
    {
        vkDestroyImageView(getDevice(), view, nullptr);
    }

    vkDestroySwapchainKHR(getDevice(), mSwapchain.handle, nullptr);

    mSwapchain = {};
}

void Renderer::DestroyRenderPass() 
{
    CHROME_TRACE_FUNCTION();
    mRenderPass.Destroy();
}

void Renderer::DestroySwapchainFramebuffers() 
{
    CHROME_TRACE_FUNCTION();
    for (VkFramebuffer framebuffer : mSwapchainFramebuffer) 
    {
        vkDestroyFramebuffer(getDevice(), framebuffer, nullptr);
    }

    mSwapchainFramebuffer.clear();
}

void Renderer::DestroySemaphores() 
{
    CHROME_TRACE_FUNCTION();
    vkDestroySemaphore(getDevice(), mSemaphores.imageAcquired, nullptr);    
    vkDestroySemaphore(getDevice(), mSemaphores.renderingFinish, nullptr);    
}

void Renderer::DestroyCommandBuffers() 
{
    CHROME_TRACE_FUNCTION();
    mCommandBuffers.render.Destroy();    
}

void Renderer::SetUniformCameraData(UniformData& data, const Camera& camera) 
{
    data.projection = camera.GetProjection();    
    data.view = camera.GetView();
    data.cameraPosition = camera.GetPosition();
    data.cameraFront = camera.GetFront();
}

void Renderer::UpdateMaterialDescriptorSet(const std::vector<DrawSubmitInfo>& drawSubmitInfos, UniformBuffer& uniformBuffer, UniformData& uniformData) 
{
    for(auto drawSubmit : drawSubmitInfos)
    {
        assert(drawSubmit.mesh->IsValid());
        assert(drawSubmit.material->IsValid());

        uniformBuffer.SetDataToDescriptor(sizeof(UniformData), &uniformData, drawSubmit.material->mDescriptorSet, 0);

        if(drawSubmit.material->mAlbedo.IsValid())
            drawSubmit.material->mAlbedo.SetDataToDescriptorSet(drawSubmit.material->mDescriptorSet, 1);
    }
}

void Renderer::CmdDrawSubmitBindDescriptorSet(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo) 
{
    vkCmdBindDescriptorSets(mCommandBuffers.render.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, drawSubmitInfo.material->mPipeline.GetPipelineLayout(), 0, 1, &drawSubmitInfo.material->mDescriptorSet, 0, nullptr);
}

void Renderer::CmdDrawSubmitBindPipeline(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo) 
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawSubmitInfo.material->mPipeline.GetHandle());
}

void Renderer::CmdDrawSubmitBindVertexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo) 
{
    VkBuffer vertexBuffers[2]; 
    uint32_t vertexBufferCount = 1;
    vertexBuffers[0] = drawSubmitInfo.mesh->mVertexBuffer.handle;
    if(drawSubmitInfo.material->mSettings.enableInstancing)
    {
        vertexBufferCount = 2;
        vertexBuffers[1] = drawSubmitInfo.instanceBuffer->mBuffer.handle;
    }
    VkDeviceSize offsets[] = {0, 0};

    vkCmdBindVertexBuffers(mCommandBuffers.render.GetHandle(), 0, vertexBufferCount, vertexBuffers, offsets);
}

void Renderer::CmdDrawSubmitBindIndexBuffer(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo) 
{
    vkCmdBindIndexBuffer(mCommandBuffers.render.GetHandle(), drawSubmitInfo.mesh->mIndexBuffer.handle, 0, VK_INDEX_TYPE_UINT32);
}

void Renderer::RenderDrawSubmitInfos(const std::vector<DrawSubmitInfo>& drawSubmitInfos) 
{
    for (int i = 0; i < drawSubmitInfos.size(); i++)
    {
        DrawSubmitInfo drawSubmit = drawSubmitInfos[i];

        int previousIndex = glm::clamp(i - 1, 0, INT32_MAX);

        if(i == 0 || drawSubmit.material != drawSubmitInfos[previousIndex].material)
        {
            CmdDrawSubmitBindDescriptorSet(mCommandBuffers.render.GetHandle(), drawSubmit);
            CmdDrawSubmitBindPipeline(mCommandBuffers.render.GetHandle(), drawSubmit);
        }
        if(i == 0 || drawSubmit.mesh != drawSubmitInfos[previousIndex].mesh || drawSubmit.instanceBuffer != drawSubmitInfos[previousIndex].instanceBuffer)
        {
            CmdDrawSubmitBindVertexBuffer(mCommandBuffers.render.GetHandle(), drawSubmit);
            CmdDrawSubmitBindIndexBuffer(mCommandBuffers.render.GetHandle(), drawSubmit);
        }

        glm::mat4 model = drawSubmit.transform.GetMatrix();
        vkCmdPushConstants(mCommandBuffers.render.GetHandle(), drawSubmit.material->mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &model);

        if(drawSubmit.material->mSettings.enableInstancing)
            vkCmdDrawIndexed(mCommandBuffers.render.GetHandle(), drawSubmit.mesh->mIndexSize / sizeof(uint32_t), drawSubmit.instanceCount, 0, 0, 0);
        else
            vkCmdDrawIndexed(mCommandBuffers.render.GetHandle(), drawSubmit.mesh->mIndexSize / sizeof(uint32_t), 1, 0, 0, 0);
    }
}

void Renderer::PresentImage(VkQueue queue, const Swapchain& swapchain, uint32_t imageIndex, VkSemaphore waitSemaphore) 
{
    uint32_t waitSemaphoreCount = (waitSemaphore == VK_NULL_HANDLE) ? 0 : 1;

    VkPresentInfoKHR presentInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = waitSemaphoreCount,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = &swapchain.handle,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(queue, &presentInfo);

}
