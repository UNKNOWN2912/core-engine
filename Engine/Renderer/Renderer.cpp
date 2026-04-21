#include "Renderer.hpp"
#include "Core/Macro.hpp"
#include "GLFW/glfw3.h"
#include "Renderer/Transform.hpp"
#include "Utility.hpp"
#include "GraphicsContext.hpp"
#include <glm/gtc/matrix_transform.hpp>

void Renderer::Initialize(const Window& window) 
{
    CHROME_TRACE_FUNCTION();

    mContext.Create(window, true);
    mSwapchain.Create(window.GetSize(), PresentMode::Fifo);
    CreateRenderPass();
    CreateSwapchainFramebuffers();
    CreateShadowMapObjects();
    CreateSemaphores();
    CreateCommandBuffers();
    mUniformBuffer.Create(sizeof(UniformData));
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

    // mUniformData.view = glm::lookAt(glm::vec3(100.f), glm::vec3(0,64,0), glm::vec3(0,1,0));
    // mUniformData.projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, -150.f, 150.f);
    // mUniformData.projection[1][1] *= -1;

    
    mUniformData.time = glfwGetTime();

    UpdateMaterialDescriptorSet(mDrawSubmitInfo, mUniformBuffer, mUniformData);

    vkDeviceWaitIdle(getDevice());
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), mSwapchain.GetHandle(), UINT64_MAX, mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);

    mCommandBuffers.render.BeginRecording();
    
    CmdShadowRenderPass();
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

void Renderer::CmdMainRenderPass(uint32_t imageIndex) 
{
    CHROME_TRACE_FUNCTION();

    mRenderPass.CmdBeginRenderPass(mCommandBuffers.render, mSwapchainFramebuffer[imageIndex], {mSwapchain.GetSize().x, mSwapchain.GetSize().y}, {{1,1,1,1}, {1,0,0,1}});
    
    VkRect2D scissor = 
    {
        .extent = {(uint32_t)mViewport.width, (uint32_t)mViewport.height},
    };

    vkCmdSetViewport(mCommandBuffers.render.GetHandle(), 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffers.render.GetHandle(), 0, 1, &scissor);
    
    RenderDrawSubmitInfos(mDrawSubmitInfo);

    mRenderPass.CmdEndRenderPass(mCommandBuffers.render);
}

void Renderer::CmdShadowRenderPass() 
{
    // lightDirection = glm::vec3(sin(glfwGetTime() * 0.05), cos(glfwGetTime() * 0.05), 0);
    mShadowObjects.uniformData.view = glm::lookAt(lightDirection + mCamera.GetPosition(), glm::vec3(0,0,0) + mCamera.GetPosition(), glm::vec3(0,1,0));
    mShadowObjects.uniformData.projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, -150.f, 150.f);
    mShadowObjects.uniformData.projection[1][1] *= -1;

    mShadowObjects.uniformBuffer.SetData(sizeof(ShadowUniformData), &mShadowObjects.uniformData);
    mShadowObjects.uniformBuffer.UpdateDescriptor(mShadowObjects.descriptorSet, 0);

    mShadowObjects.renderPass.CmdBeginRenderPass(mCommandBuffers.render, mShadowObjects.framebuffer, mShadowObjects.image.size, {{1,0,0,1}});

        VkViewport viewport = 
    {
        .x = 0,
        .y = 0,
        .width = (float)mShadowObjects.image.size.x,
        .height = (float)mShadowObjects.image.size.y,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    VkRect2D scissor = 
    {
        .extent = {(uint32_t)viewport.width, (uint32_t)viewport.height},
    };


    vkCmdSetViewport(mCommandBuffers.render.GetHandle(), 0, 1, &viewport);
    vkCmdSetScissor(mCommandBuffers.render.GetHandle(), 0, 1, &scissor);


    RenderDrawSubmitInfosForShadowMap(mDrawSubmitInfo);
    
    mShadowObjects.renderPass.CmdEndRenderPass(mCommandBuffers.render);
}

void Renderer::CreateShadowMapObjects() 
{
    CHROME_TRACE_FUNCTION();


    VkSamplerCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VK_FILTER_NEAREST,
        .minFilter = VK_FILTER_NEAREST,
        .addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
        .minLod = 1,
        .maxLod = 1,
        .borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
    };

    vkCreateSampler(getDevice(), &createInfo, nullptr, &mShadowObjects.sampler);

    mShadowObjects.renderPass.AddAttachment(ImageFormat::D32, ImageLayout::ShaderRead, LoadOperation::Clear, StoreOperation::Store);
    mShadowObjects.renderPass.AddSubpass({}, {}, 0);
    mShadowObjects.renderPass.AddDependency(RenderPass::ExternalSubpass, 0, PipelineStage::EarlyFragmentTests | PipelineStage::LateFragmentTests, PipelineStage::LateFragmentTests);
    mShadowObjects.renderPass.Create();

    mShadowObjects.image = CreateImage({2048, 2048}, ImageFormat::D32, ImageUsage::DepthStencil | ImageUsage::Sampler, ImageAspect::Depth, MemoryProperty::DeviceLocal);
    mShadowObjects.framebuffer = CreateFramebuffer({mShadowObjects.image.size}, {mShadowObjects.image}, mShadowObjects.renderPass);

    VkDescriptorSetLayoutBinding shadowUniformBinding = 
    {
        .binding = 0,
        .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
    };

    VkDescriptorPoolSize uniformBinding = 
    {
        .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        .descriptorCount = 1,
    };

    VkPushConstantRange pushConstantRange = 
    {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .offset = 0,
        .size = sizeof(glm::mat4),
    };

    mShadowObjects.setLayout = CreateDescriptorSetLayout({shadowUniformBinding});
    mShadowObjects.descriptorPool = CreateDescriptorPool({uniformBinding}, 1);
    mShadowObjects.descriptorSet = AllocateDescriptorSet(mShadowObjects.setLayout, mShadowObjects.descriptorPool);

    mShadowObjects.pipeline.AddBinding(0, sizeof(Vertex), InputRate::Vertex);
    mShadowObjects.pipeline.AddAttribute(0, 0, ImageFormat::RGB32, offsetof(Vertex, position));
    mShadowObjects.pipeline.AddAttribute(0, 1, ImageFormat::RG32, offsetof(Vertex, uv));
    mShadowObjects.pipeline.AddAttribute(0, 2, ImageFormat::RGB32, offsetof(Vertex, normal));
    mShadowObjects.pipeline.AddBinding(1, sizeof(glm::mat4), InputRate::Instance);
    mShadowObjects.pipeline.AddAttribute(1, 3, ImageFormat::RGBA32, sizeof(glm::vec4) * 0);
    mShadowObjects.pipeline.AddAttribute(1, 4, ImageFormat::RGBA32, sizeof(glm::vec4) * 1);
    mShadowObjects.pipeline.AddAttribute(1, 5, ImageFormat::RGBA32, sizeof(glm::vec4) * 2);
    mShadowObjects.pipeline.AddAttribute(1, 6, ImageFormat::RGBA32, sizeof(glm::vec4) * 3);
    mShadowObjects.pipeline.SetPipelineLayout(CreatePipelineLayout({mShadowObjects.setLayout}, {pushConstantRange}));
    mShadowObjects.pipeline.EnableDepthTesting(true);
    mShadowObjects.pipeline.EnableDepthWrite(true);
    mShadowObjects.pipeline.LoadVertexShader("Shaders/shadow.vert.spv");
    mShadowObjects.pipeline.LoadFragmentShader("Shaders/shadow.frag.spv");
    mShadowObjects.pipeline.SetCullMode(CullMode::None);
    mShadowObjects.pipeline.Create(mShadowObjects.renderPass, 0);

    mShadowObjects.uniformBuffer.Create(sizeof(mShadowObjects.uniformData));
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

    mDepthAttachment = CreateImage(mSwapchain.GetSize(), ImageFormat::D32, ImageUsage::DepthStencil, ImageAspect::Depth, MemoryProperty::DeviceLocal);

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

void Renderer::SetUniformCameraData(UniformData& data, const Camera& camera) 
{
    data.projection = camera.GetProjection();    
    data.view = camera.GetView();
    data.cameraPosition = camera.GetPosition();
    data.cameraFront = camera.GetFront();
}

void Renderer::UpdateMaterialDescriptorSet(const std::vector<DrawSubmitInfo>& drawSubmitInfos, UniformBuffer& uniformBuffer, UniformData& uniformData) 
{
    uniformData.time = glfwGetTime();

    uniformBuffer.SetData(sizeof(UniformData), &uniformData);

    for(auto drawSubmit : drawSubmitInfos)
    {
        assert(drawSubmit.mesh->IsValid());
        assert(drawSubmit.material->IsValid());

        uniformBuffer.UpdateDescriptor(drawSubmit.material->mDescriptorSet, 0);

        if(drawSubmit.material->mAlbedo.IsValid())
            drawSubmit.material->mAlbedo.UpdateDescriptorSet(drawSubmit.material->mDescriptorSet, 1);

        mShadowObjects.uniformBuffer.UpdateDescriptor(drawSubmit.material->mDescriptorSet, 2);

        VkDescriptorImageInfo imageInfo = 
        {
            .sampler = mShadowObjects.sampler,
            .imageView = mShadowObjects.image.view,
            .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        };

        VkWriteDescriptorSet write = 
        {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .dstSet = drawSubmit.material->mDescriptorSet,
            .dstBinding = 3,
            .dstArrayElement = 0,
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
            .pImageInfo = &imageInfo,
        };

        vkUpdateDescriptorSets(getDevice(), 1, &write, 0, nullptr);
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

void Renderer::RenderDrawSubmitInfosForShadowMap(const std::vector<DrawSubmitInfo>& drawSubmitInfos) 
{
    // CmdDrawSubmitBindDescriptorSet(mCommandBuffers.render.GetHandle(), drawSubmit);
    // CmdDrawSubmitBindPipeline(mCommandBuffers.render.GetHandle(), drawSubmit);


    vkCmdBindDescriptorSets(mCommandBuffers.render.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mShadowObjects.pipeline.GetPipelineLayout(), 0, 1, &mShadowObjects.descriptorSet, 0, nullptr);
    vkCmdBindPipeline(mCommandBuffers.render.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mShadowObjects.pipeline.GetHandle());




    for (int i = 0; i < drawSubmitInfos.size(); i++)
    {
        DrawSubmitInfo drawSubmit = drawSubmitInfos[i];

        int previousIndex = glm::clamp(i - 1, 0, INT32_MAX);

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
