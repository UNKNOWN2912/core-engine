#include "Renderer.hpp"
#include "Utility.hpp"
#include "GraphicsContext.hpp"
#include <print>

void Renderer::Initialize(const Window& window) 
{
    mContext.Create(window, true);
    CreateSwapchain();
    CreateRenderPass();
    CreateSwapchainFramebuffers();
    CreateSemaphores();
    CreateCommandBuffers();
    CreatePipelines();
}

void Renderer::Terminate() 
{
    vkDeviceWaitIdle(getDevice());
    DestroyCommandBuffers();
    DestroySemaphores();
    DestroySwapchainFramebuffers();
    DestroyRenderPass();
    DestroySwapchain();
    mContext.Destroy();
}

void Renderer::DrawMesh(StaticMesh& mesh)
{
    mMeshQueue.push_back(&mesh);
}

void Renderer::BeginFrame() 
{
    mFrameRunning = true;
}

void Renderer::EndFrame() 
{
    vkDeviceWaitIdle(getDevice());

    uint32_t imageIndex;
    vkAcquireNextImageKHR(getDevice(), mSwapchain.handle, UINT64_MAX, mSemaphores.imageAcquired, VK_NULL_HANDLE, &imageIndex);

    VkCommandBufferBeginInfo beginInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    };

    vkBeginCommandBuffer(mCommandBuffers.renderingCommandBuffer, &beginInfo);

    VkClearValue clearValue = 
    {
        .color = {1,0,1,1},
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
        .clearValueCount = 1,
        .pClearValues = &clearValue,
    };

    vkCmdBeginRenderPass(mCommandBuffers.renderingCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


    for (size_t i = 0; i < mMeshQueue.size(); i++)
    {
        StaticMesh& mesh = *mMeshQueue[i];
        VkBuffer vertexBuffers[] = {mesh.mVertexBuffer.handle};
        VkBuffer indexBuffer = mesh.mIndexBuffer.handle;

        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(mCommandBuffers.renderingCommandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(mCommandBuffers.renderingCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindPipeline(mCommandBuffers.renderingCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mDefaultPipeline.GetHandle());

        VkRect2D scissor = 
        {
            .extent = {(uint32_t)mViewport.width, (uint32_t)mViewport.height},
        };

        vkCmdSetViewport(mCommandBuffers.renderingCommandBuffer, 0, 1, &mViewport);
        vkCmdSetScissor(mCommandBuffers.renderingCommandBuffer, 0, 1, &scissor);

        vkCmdDrawIndexed(mCommandBuffers.renderingCommandBuffer, 3, 1, 0, 0, 0);
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

    mMeshQueue.clear();

    mFrameRunning = false;
}

void Renderer::CreateSwapchain() 
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(getPhysicalDevice(), getSurface(), &capabilities);
    mSwapchain.extent = capabilities.currentExtent;

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

    VkAttachmentReference swapchainAttachmentReference = 
    {
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = 
    {
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .colorAttachmentCount = 1,
        .pColorAttachments = &swapchainAttachmentReference,
    };

    VkSubpassDependency dependencies = 
    {
        .srcSubpass = VK_SUBPASS_EXTERNAL,
        .dstSubpass = 0,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
        .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
    };

    VkRenderPassCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .attachmentCount = 1,
        .pAttachments = &swapchainAttachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &dependencies,
    };

    vkCreateRenderPass(getDevice(), &createInfo, nullptr, &mRenderPass);
}

void Renderer::CreateSwapchainFramebuffers() 
{
    VkFramebufferCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .renderPass = mRenderPass,
        .attachmentCount = 1,
        .width = mSwapchain.extent.width,
        .height = mSwapchain.extent.height,
        .layers = 1,
    };

    for(VkImageView view : mSwapchain.views)
    {
        createInfo.pAttachments = &view;
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
    VkSemaphoreCreateInfo createInfo = 
    {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    };

    vkCreateSemaphore(getDevice(), &createInfo, nullptr, &mSemaphores.imageAcquired);
    vkCreateSemaphore(getDevice(), &createInfo, nullptr, &mSemaphores.renderingFinish);
}

void Renderer::CreateCommandBuffers() 
{
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
    mDefaultPipeline.AddBinding(0, sizeof(glm::vec3), VK_VERTEX_INPUT_RATE_VERTEX);
    mDefaultPipeline.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0);
    
    mDefaultPipeline.LoadVertexShader("Shader/shader.vert.spv");
    mDefaultPipeline.LoadFragmentShader("Shader/shader.frag.spv");
    
    mDefaultPipeline.AddColorBlendAttachment();

    mDefaultPipeline.Create(mRenderPass, 0);
}

void Renderer::DestroySwapchain() 
{
    for (VkImageView view : mSwapchain.views) 
    {
        vkDestroyImageView(getDevice(), view, nullptr);
    }

    vkDestroySwapchainKHR(getDevice(), mSwapchain.handle, nullptr);

    mSwapchain = {};
}

void Renderer::DestroyRenderPass() 
{
    vkDestroyRenderPass(getDevice(), mRenderPass, nullptr);    
}

void Renderer::DestroySwapchainFramebuffers() 
{
    for (VkFramebuffer framebuffer : mSwapchainFramebuffer) 
    {
        vkDestroyFramebuffer(getDevice(), framebuffer, nullptr);
    }    
}

void Renderer::DestroySemaphores() 
{
    vkDestroySemaphore(getDevice(), mSemaphores.imageAcquired, nullptr);    
    vkDestroySemaphore(getDevice(), mSemaphores.renderingFinish, nullptr);    
}

void Renderer::DestroyCommandBuffers() 
{
    vkFreeCommandBuffers(getDevice(), getCommandPool(), 1, &mCommandBuffers.renderingCommandBuffer);    
}
