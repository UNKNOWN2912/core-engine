#include "Renderer.hpp"
#include "Core/Application.hpp"
#include "Core/Macro.hpp"
#include "Renderer/Transform.hpp"
#include "Renderer/Types.hpp"
#include "Utility.hpp"
#include "GraphicsContext.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan_core.h>

void Renderer::Initialize(const Window& window) 
{
    CHROME_TRACE_FUNCTION();

    mContext.Create(window, true);
    mSwapchain.Create(window.GetSize(), PresentMode::Fifo);
    CreateRenderPass();
    CreateSwapchainFramebuffers();
    CreateFinalImageAttachment(mSwapchain.GetSize());
    CreateSemaphores();
    CreateCommandBuffers();
    mDeferredAttachments.CreateAttachments(mSwapchain.GetSize());
    mUniformBuffer.Create(sizeof(UniformData));
}

void Renderer::Terminate() 
{
    CHROME_TRACE_FUNCTION();
}


void Renderer::BeginFrame() 
{
    vkDeviceWaitIdle(getDevice());
    CHROME_TRACE_FUNCTION();
    mDrawSubmitInfo.clear();
    mFrameRunning = true;
}

void Renderer::EndFrame() 
{
    CHROME_TRACE_FUNCTION();

    mCamera.Calculate();
    UpdateUniformData();

    vkDeviceWaitIdle(getDevice());
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), mSwapchain.GetHandle(), UINT64_MAX, mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);

    mCommandBuffers.render.BeginRecording();
    
    CmdMainRenderPass(imageIndex);
    
    mCommandBuffers.render.EndRecording();

    mCommandBuffers.render.QueueSubmit(getQueues().graphics, mSemaphores.imageAcquired, mSemaphores.renderingFinish, PipelineStage::ColorAttachmentOutput);

    PresentImage(getQueues().present, mSwapchain, imageIndex, mSemaphores.renderingFinish);

    mFrameRunning = false;
}

void Renderer::Resize(const glm::uvec2& size)
{
    CHROME_TRACE_FUNCTION();
    vkDeviceWaitIdle(getDevice());

    mSwapchain.Destroy();
    DestroySwapchainFramebuffers();

    mSwapchain.Create(size, PresentMode::Fifo);
    CreateSwapchainFramebuffers();

    mViewport = {};
    mViewport.width = mSwapchain.GetSize().x;
    mViewport.height = mSwapchain.GetSize().y;
    mViewport.maxDepth = 1.f;
    mViewport.minDepth = 0.f;
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

void Renderer::CmdMainRenderPass(uint32_t imageIndex) 
{
    CHROME_TRACE_FUNCTION();

    VkRect2D scissor = 
    {
        .extent = {(uint32_t)mViewport.width, (uint32_t)mViewport.height},
    };

    
    mRenderPass.CmdBeginRenderPass(mCommandBuffers.render, mFinalFrameBuffer, {mSwapchain.GetSize().x, mSwapchain.GetSize().y}, {{1,1,1,1}, {1,0,0,1}});
    
    vkCmdSetViewport(mCommandBuffers.render.GetHandle(), 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    RenderDrawSubmitInfos(mDrawSubmitInfo);
    
    mRenderPass.CmdEndRenderPass(mCommandBuffers.render);
    
    mRenderPass.CmdBeginRenderPass(mCommandBuffers.render, mSwapchainFramebuffer[imageIndex], {mSwapchain.GetSize().x, mSwapchain.GetSize().y}, {{1,1,1,1}, {1,0,0,1}});
    
    vkCmdSetViewport(mCommandBuffers.render.GetHandle(), 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    RenderDrawSubmitInfos(mDrawSubmitInfo);
    
    mRenderPass.CmdEndRenderPass(mCommandBuffers.render);
}

void Renderer::CreateRenderPass()
{
    CHROME_TRACE_FUNCTION();

    mSwapchainRenderPass.AddAttachment(ImageFormat::BGRA8, ImageLayout::PresentSource, LoadOperation::Clear, StoreOperation::Store);
    mSwapchainRenderPass.AddSubpass({0}, {}, 1);
    mSwapchainRenderPass.AddDependency(mSwapchainRenderPass.ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput, PipelineStage::ColorAttachmentOutput);
    mSwapchainRenderPass.Create();


    mMainRenderPass.AddAttachment(ImageFormat::RGB8, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mMainRenderPass.AddAttachment(ImageFormat::RGB16, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mMainRenderPass.AddAttachment(ImageFormat::RGB32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mMainRenderPass.AddAttachment(ImageFormat::D32, ImageLayout::DepthStencil, LoadOperation::Clear, StoreOperation::Store);
    mMainRenderPass.AddSubpass({0,1,2}, {}, 3);
    mMainRenderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests, PipelineStage::ColorAttachmentOutput | PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests);
    mMainRenderPass.Create();
}

void Renderer::CreateSwapchainFramebuffers() 
{
    CHROME_TRACE_FUNCTION();

    mDepthAttachment = CreateImage(mSwapchain.GetSize(), ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal);
    mSceneDepthAttachment = CreateImage(mSwapchain.GetSize(), ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal);

    for(Image image : mSwapchain.GetImages())
    {
        VkFramebuffer framebuffer = CreateFramebuffer(mSwapchain.GetSize(), {image, mDepthAttachment}, mRenderPass);
        mSwapchainFramebuffer.push_back(framebuffer);
    }

    mViewport.width = mSwapchain.GetSize().x;
    mViewport.height = mSwapchain.GetSize().y;
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


void Renderer::UpdateMaterialDescriptorSet(const std::vector<DrawSubmitInfo>& drawSubmitInfos, UniformBuffer& uniformBuffer, UniformData& uniformData) 
{
    uniformBuffer.SetData(sizeof(UniformData), &uniformData);
}

void Renderer::CmdDrawSubmitBindDescriptorSet(VkCommandBuffer commandBuffer, const DrawSubmitInfo& drawSubmitInfo) 
{
    VkDescriptorSet descriptorSets[] = {drawSubmitInfo.material->mDescriptor.GetDescriptorSet()};
    vkCmdBindDescriptorSets(mCommandBuffers.render.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, drawSubmitInfo.material->mPipeline.GetPipelineLayout(), 0, sizeof(descriptorSets) / sizeof(VkDescriptorSet), descriptorSets, 0, nullptr);
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
    VkSwapchainKHR swapchains[] = {swapchain.GetHandle()};

    VkPresentInfoKHR presentInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = waitSemaphoreCount,
        .pWaitSemaphores = &waitSemaphore,
        .swapchainCount = 1,
        .pSwapchains = swapchains,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(queue, &presentInfo);
}

void Renderer::UpdateUniformData() 
{
    mUniformData.projection = mCamera.GetProjection();    
    mUniformData.view = mCamera.GetView();
    mUniformData.cameraPosition = mCamera.GetPosition();
    mUniformData.cameraFront = mCamera.GetFront();
    mUniformData.time = Application::GetInstance()->GetElapsedTime();
    mUniformBuffer.SetData(sizeof(UniformData), &mUniformData);
}
void Renderer::CreateFinalImageAttachment(const glm::uvec2& size) 
{
    mFinalImage = CreateImage(size, ImageFormat::BGRA8, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mSceneDepthAttachment = CreateImage(size, ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal);
    mFinalFrameBuffer = CreateFramebuffer(size, {mFinalImage, mSceneDepthAttachment}, mRenderPass);
}



void DeferredAttachment::CreateAttachments(const glm::uvec2& size) 
{
    mSize = size;

    mPosition = CreateImage(mSize, ImageFormat::RGBA32, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mAlbedo = CreateImage(mSize, ImageFormat::RGBA8, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mNormal = CreateImage(mSize, ImageFormat::RGBA16, ImageUsage::Color | ImageUsage::Sampler, ImageAspect::Color, MemoryProperty::DeviceLocal);
    mDepth = CreateImage(mSize, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal);
}

void DeferredAttachment::DestroyAttachments() 
{
    DestroyImage(mPosition);
    DestroyImage(mAlbedo);
    DestroyImage(mNormal);
    DestroyImage(mDepth);
}

void DeferredAttachment::ResizeAttachments(const glm::uvec2& size) 
{
    if(mSize == size)
        return;

    DestroyAttachments();
    CreateAttachments(size);
}