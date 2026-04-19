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
    CreatePipelines();
}

void Renderer::Terminate() 
{
    CHROME_TRACE_FUNCTION();
    // DestroyCommandBuffers();
    // DestroySemaphores();
    // DestroySwapchainFramebuffers();
    // DestroyRenderPass();
    // DestroySwapchain();
    // mContext.Destroy();
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

    mUniformData.projection = mCamera.GetProjection();
    mUniformData.projection[1][1] *= -1; 
    mUniformData.view = mCamera.GetView();
    mUniformData.cameraPosition = mCamera.GetPosition();
    mUniformData.cameraFront = mCamera.GetFront();

    mUniformData.time = glfwGetTime();

    for(auto drawSubmit : mDrawSubmitInfo)
    {
        assert(drawSubmit.mesh->IsValid());
        assert(drawSubmit.material->IsValid());
        mUniformBuffer.SetDataToDescriptor(sizeof(UniformData), &mUniformData, drawSubmit.material->mDescriptorSet, 0);

        if(drawSubmit.material->mAlbedo.IsValid())
            drawSubmit.material->mAlbedo.SetDataToDescriptorSet(drawSubmit.material->mDescriptorSet, 1);
    }


    
    vkDeviceWaitIdle(getDevice());
    
    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), mSwapchain.handle, UINT64_MAX, mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);
    
    VkCommandBufferBeginInfo beginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };
    
    vkBeginCommandBuffer(mCommandBuffers.renderingCommandBuffer, &beginInfo);
    
    VkClearValue clearValue[] = 
    {
        {{0.1,0.1,0.1,1}},
        {{1,0,0,1},},
    };
    
    VkRenderPassBeginInfo renderPassBeginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = mRenderPass,
        .framebuffer = mSwapchainFramebuffer[imageIndex],
        .renderArea = 
        {
            .offset = {0,0},
            .extent = mSwapchain.extent,
        },
        .clearValueCount = sizeof(clearValue) / sizeof(VkClearValue),
        .pClearValues = clearValue,
    };
    
    vkCmdBeginRenderPass(mCommandBuffers.renderingCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkRect2D scissor = 
    {
        .extent = {(uint32_t)mViewport.width, (uint32_t)mViewport.height},
    };

    vkCmdSetViewport(mCommandBuffers.renderingCommandBuffer, 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffers.renderingCommandBuffer, 0, 1, &scissor);
    
    for (int i = 0; i < mDrawSubmitInfo.size(); i++)
    {
        DrawSubmitInfo drawSubmit = mDrawSubmitInfo[i];

        uint32_t vertexBufferCount = 1;

        VkBuffer vertexBuffers[2] = {drawSubmit.mesh->mVertexBuffer.handle};
        if(drawSubmit.material->mSettings.enableInstancing)
        {
            vertexBufferCount = 2;
            vertexBuffers[1] = drawSubmit.instanceBuffer->mBuffer.handle;
        }

        VkBuffer indexBuffer = drawSubmit.mesh->mIndexBuffer.handle;

        VkDeviceSize offsets[] = {0, 0};

        VkDescriptorSet descriptorSets[] = {drawSubmit.material->mDescriptorSet};

        int previousIndex = (i-1 < 0) ? 0 : i - 1;

        if(i == 0 || drawSubmit.material != mDrawSubmitInfo[previousIndex].material)
        {
            vkCmdBindDescriptorSets(mCommandBuffers.renderingCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawSubmit.material->mPipeline.GetPipelineLayout(), 0, sizeof(descriptorSets) / sizeof(VkDescriptorSet), descriptorSets, 0, nullptr);
            vkCmdBindPipeline(mCommandBuffers.renderingCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, drawSubmit.material->mPipeline.GetHandle());
        }
        if(i == 0 || drawSubmit.mesh != mDrawSubmitInfo[previousIndex].mesh || drawSubmit.instanceBuffer != mDrawSubmitInfo[previousIndex].instanceBuffer)
        {
            vkCmdBindVertexBuffers(mCommandBuffers.renderingCommandBuffer, 0, vertexBufferCount, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(mCommandBuffers.renderingCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }

        glm::mat4 model = drawSubmit.transform.GetMatrix();
        vkCmdPushConstants(mCommandBuffers.renderingCommandBuffer, drawSubmit.material->mPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &model);

        if(drawSubmit.material->mSettings.enableInstancing)
            vkCmdDrawIndexed(mCommandBuffers.renderingCommandBuffer, drawSubmit.mesh->mIndexSize / sizeof(uint32_t), drawSubmit.instanceCount, 0, 0, 0);
        else
            vkCmdDrawIndexed(mCommandBuffers.renderingCommandBuffer, drawSubmit.mesh->mIndexSize / sizeof(uint32_t), 1, 0, 0, 0);
    }
    vkCmdEndRenderPass(mCommandBuffers.renderingCommandBuffer);

    vkEndCommandBuffer(mCommandBuffers.renderingCommandBuffer);

    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &mSemaphores.imageAcquired,
        .pWaitDstStageMask = &waitStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &mCommandBuffers.renderingCommandBuffer,
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &mSemaphores.renderingFinish,
    };

    vkQueueSubmit(getQueues().graphics, 1, &submitInfo, VK_NULL_HANDLE);

    VkPresentInfoKHR presentInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &mSemaphores.renderingFinish,
        .swapchainCount = 1,
        .pSwapchains = &mSwapchain.handle,
        .pImageIndices = &imageIndex,
    };

    vkQueuePresentKHR(getQueues().present, &presentInfo);

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
        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.format = mSwapchain.format;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;
        viewCreateInfo.subresourceRange.layerCount = 1;
        viewCreateInfo.subresourceRange.levelCount = 1;

        VkImageView view;
        vkCreateImageView(getDevice(), &viewCreateInfo, nullptr, &view);
        mSwapchain.views.push_back(view);
    } 
}

void Renderer::CreateRenderPass()
{
    CHROME_TRACE_FUNCTION();
    VkAttachmentDescription swapchainAttachmentDescription = 
    {
        .flags = 0,
        .format = mSwapchain.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
    };

    VkAttachmentDescription depthAttachmentDescription = 
    {
        .flags = 0,
        .format = VK_FORMAT_D32_SFLOAT,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference swapchainAttachmentReference = 
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkAttachmentReference depthAttachmentReference = 
    {
        .attachment = 1,
        .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = 
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &swapchainAttachmentReference,
        .pDepthStencilAttachment = &depthAttachmentReference,
    };

    VkSubpassDependency dependencies = 
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
        .srcAccessMask = 0,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
    };

    VkAttachmentDescription attachmentDescriptions[] = {swapchainAttachmentDescription, depthAttachmentDescription};

    VkRenderPassCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 2,
        .pAttachments = attachmentDescriptions,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &dependencies,
    };

    vkCreateRenderPass(getDevice(), &createInfo, nullptr, &mRenderPass);
}

void Renderer::CreateSwapchainFramebuffers() 
{
    CHROME_TRACE_FUNCTION();
    VkFramebufferCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = mRenderPass,
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
    VkCommandBufferAllocateInfo allocateInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .commandPool = getCommandPool(),
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    vkAllocateCommandBuffers(getDevice(), &allocateInfo, &mCommandBuffers.renderingCommandBuffer);
}

void Renderer::CreatePipelines() 
{
    CHROME_TRACE_FUNCTION();
    // mDefaultPipeline.AddBinding(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
    // mDefaultPipeline.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position));
    // mDefaultPipeline.AddAttribute(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv));
    // mDefaultPipeline.AddAttribute(0, 2, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal));
    // mDefaultPipeline.AddColorBlendAttachment();

    // mDefaultPipeline.EnableDepth(true);
    
    // mDefaultPipeline.LoadVertexShader("Shader/shader.vert.spv");
    // mDefaultPipeline.LoadFragmentShader("Shader/shader.frag.spv");

    // mTexture.Load("Texture/texture_01.png", 1);
    
    // VkPipelineLayout pipelineLayout = CreatePipelineLayout({mUniformBuffer.GetSetLayout(), mTexture.GetSetLayout()});
    // mDefaultPipeline.SetPipelineLayout(pipelineLayout);

    // mDefaultPipeline.SetCullMode(VK_CULL_MODE_NONE);

    // mDefaultPipeline.Create(mRenderPass, 0);
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
    vkDestroyRenderPass(getDevice(), mRenderPass, nullptr);    
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
    vkFreeCommandBuffers(getDevice(), getCommandPool(), 1, &mCommandBuffers.renderingCommandBuffer);    
}
