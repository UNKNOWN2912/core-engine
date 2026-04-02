#include <Renderer/Graphic.hpp>
#include "Renderer/Vulkan/VulkanSwapchain.hpp"
#include <cstring>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "VulkanHelper.hpp"


struct ImageData
{
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
};

struct GraphicData
{
    VkInstance instance = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VulkanSwapchain swapchain;
    QueueIndex queueIndices;
    Queue queues;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
};


void Graphic::Initialize(const Window& window)
{
    if(mData != nullptr)
        Clean();
    
    mData = new GraphicData;
    CreateVulkanObjects(window);
}

void Graphic::Terminate() 
{
    Clean();
}

void Graphic::Clean() 
{
    DestroyVulkanObjects();
    delete mData;
}

void Graphic::CreateVulkanObjects(const Window& window) 
{
    mData->instance = VulkanHelper::CreateInstance(true);
    mData->physicalDevice = VulkanHelper::GetPhysicalDevice(mData->instance);
    mData->surface = VulkanHelper::CreateSurface(mData->instance, window);
    mData->queueIndices = VulkanHelper::GetQueueIndices(mData->physicalDevice, mData->surface);
    mData->device = VulkanHelper::CreateDevice(mData->physicalDevice, mData->queueIndices);
    mData->queues = VulkanHelper::GetQueues(mData->device, mData->queueIndices);
    mData->commandPool = VulkanHelper::CreateCommandPool(mData->device);
    mData->swapchain = VulkanHelper::CreateSwapchain(mData->physicalDevice, mData->device, mData->surface, window);
}

void Graphic::DestroyVulkanObjects() 
{
    mData->swapchain.Destroy();
    vkDestroySurfaceKHR(mData->instance, mData->surface, nullptr);
    vkDestroyDevice(mData->device, nullptr);
    vkDestroyCommandPool(mData->device, mData->commandPool, nullptr);
    vkDestroyInstance(mData->instance, nullptr);    
}



void Graphic::CreateRenderPass(RenderPass& renderPass)
{
    std::vector<VkAttachmentDescription> attachmentDescriptions;
    std::vector<VkSubpassDescription> subpassDescriptions;
    std::vector<VkSubpassDependency> subpassDependencies;

    for (size_t i = 0; i < renderPass.mAttachments.size(); i++)
    {
        const Attachment& attachment = renderPass.mAttachments[i];
        VkAttachmentDescription description = VulkanHelper::GetVulkanAttachmentFromAttachment(attachment);
        attachmentDescriptions.push_back(description); 
    }

    for (size_t i = 0; i < renderPass.mSubpasses.size(); i++)
    {
        const Subpass& subpass = renderPass.mSubpasses[i];
        const Dependency& dependency = renderPass.mDependencies[i];

        std::vector<VkAttachmentReference> colorAttachmentRefs;
        std::vector<VkAttachmentReference> inputAttachmentRefs;
        VkAttachmentReference depthAttachmentRef;
        
        for (size_t j = 0; j < subpass.mColorAttachmentIndex.size(); j++)
        {
            uint32_t index = subpass.mColorAttachmentIndex[j];
            VkAttachmentReference reference = {};
            reference.attachment = index;
            reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentRefs.push_back(reference);
        }

        for (size_t j = 0; j < subpass.mInputAttachmentIndex.size(); j++)
        {
            uint32_t index = subpass.mInputAttachmentIndex[j];
            VkAttachmentReference reference = {};
            reference.attachment = index;
            reference.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            inputAttachmentRefs.push_back(reference);
        }

        {
            uint32_t index = subpass.mDepthAttachmentIndex;
            VkAttachmentReference reference = {};
            reference.attachment = index;
            reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depthAttachmentRef = reference;
        }

        VkAttachmentReference* colorAttachmentRefData = new VkAttachmentReference[colorAttachmentRefs.size()];
        memcpy(colorAttachmentRefData, colorAttachmentRefs.data(), sizeof(VkAttachmentReference) * colorAttachmentRefs.size());

        VkAttachmentReference* inputAttachmentRefData = new VkAttachmentReference[inputAttachmentRefs.size()];
        memcpy(inputAttachmentRefData, inputAttachmentRefs.data(), sizeof(VkAttachmentReference) * inputAttachmentRefs.size());

        VkAttachmentReference* depthAttachmentRefData = new VkAttachmentReference[1];
        memcpy(depthAttachmentRefData, &depthAttachmentRef, sizeof(VkAttachmentReference) * 1);


        VkSubpassDescription subpassDescription = {};

        subpassDescription.colorAttachmentCount = colorAttachmentRefs.size();
        subpassDescription.pColorAttachments = colorAttachmentRefData;
        subpassDescription.inputAttachmentCount = inputAttachmentRefs.size();
        subpassDescription.pInputAttachments = inputAttachmentRefData;
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.pDepthStencilAttachment = depthAttachmentRefData;

        VkSubpassDependency subpassDependency = {};
        subpassDependency.dstSubpass = dependency.mDestinationSubpassIndex;
        subpassDependency.srcSubpass = dependency.mSourceSubpassIndex == UINT32_MAX ? VK_SUBPASS_EXTERNAL : dependency.mSourceSubpassIndex;

        if(colorAttachmentRefs.size() != 0)
        {
            subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            subpassDependency.srcAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
            subpassDependency.dstAccessMask |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        }
        if(inputAttachmentRefs.size() != 0)
        {
            subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            subpassDependency.srcAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
            subpassDependency.dstAccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        }
        if(depthAttachmentRef.layout != 0)
        { 
            subpassDependency.srcStageMask |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            subpassDependency.srcAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
            subpassDependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        }

        subpassDescriptions.push_back(subpassDescription);
        subpassDependencies.push_back(subpassDependency);
    }

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.subpassCount = subpassDescriptions.size();
    createInfo.pSubpasses = subpassDescriptions.data();
    createInfo.attachmentCount = attachmentDescriptions.size();
    createInfo.pAttachments = attachmentDescriptions.data();
    createInfo.dependencyCount = subpassDependencies.size();
    createInfo.pDependencies = subpassDependencies.data();

    VkRenderPass vkRenderPass;
    vkCreateRenderPass(mData->device, &createInfo, nullptr, &vkRenderPass);
    
    renderPass.mId = (uint64_t)vkRenderPass;

    for (size_t i = 0; i < subpassDescriptions.size(); i++)
    {
        delete subpassDescriptions[i].pColorAttachments;
        delete subpassDescriptions[i].pInputAttachments;
        delete subpassDescriptions[i].pDepthStencilAttachment;
    }
}


void Graphic::BeginCommandBufferRecording(const CommandBuffer& commandBuffer) 
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer((VkCommandBuffer)commandBuffer.mId, &beginInfo);
}

void Graphic::EndCommandBufferRecording(const CommandBuffer& commandBuffer) 
{
    vkEndCommandBuffer((VkCommandBuffer)commandBuffer.mId);
}

void Graphic::BeginRenderPass(const CommandBuffer& commandBuffer, const RenderPass& renderPass, const FrameBuffer& frameBuffer, const Viewport& viewport) 
{
    uint32_t clearColorCount = 0;
    VkClearValue clearColors[16];
    for (size_t i = 0; i < renderPass.mAttachments.size(); i++)
    {
        const Attachment& attachment = renderPass.mAttachments[i];
        if(attachment.loadOperation == LoadOperation::Clear)
        {
            glm::vec4 color = attachment.clearValue;
            clearColors[clearColorCount].color = {{color.r, color.g, color.b, color.a}};
            clearColorCount++;
        }
    }

    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderArea.extent = {viewport.size.x, viewport.size.y};
    beginInfo.renderArea.offset = {int32_t(viewport.position.x), int32_t(viewport.position.y)};
    beginInfo.clearValueCount = clearColorCount;
    beginInfo.pClearValues = clearColors;
    beginInfo.framebuffer = (VkFramebuffer)frameBuffer.mId;
    beginInfo.renderPass = (VkRenderPass)renderPass.mId;

    vkCmdBeginRenderPass((VkCommandBuffer)commandBuffer.mId, &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void Graphic::EndRenderPass(const CommandBuffer& commandBuffer) 
{
    vkCmdEndRenderPass((VkCommandBuffer)commandBuffer.mId);
}

void Graphic::BindVertexBuffer(const VertexBuffer& vertexBuffer, uint32_t bindIndex) 
{
    
}

void Graphic::BindIndexBuffer(const IndexBuffer& indexBuffer) 
{
    
}

void Graphic::DrawIndexed(uint32_t index) 
{
    
}

void Graphic::CreateCommandBuffer(CommandBuffer& commandBuffer) 
{
    VkCommandBuffer cmdBuffer;
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = mData->commandPool;

    vkAllocateCommandBuffers(mData->device, &allocateInfo, &cmdBuffer);

    commandBuffer.mId = (uint64_t)cmdBuffer;
}

void Graphic::CreateFrameBuffer(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport) 
{
    for (size_t i = 0; i < renderPass.mAttachments.size(); i++)
    {
        const Attachment& attachment = renderPass.mAttachments[i];
        DeviceImage image;
        image.mFormat = attachment.format;
        image.mSize = viewport.size;
        image.mUsage = attachment.usage;

        CreateDeviceImage(image);

        frameBuffer.mAttachments.push_back(image);
    }

    std::vector<VkImageView> vkAttachment;

    for (size_t i = 0; i < frameBuffer.mAttachments.size(); i++)
    {
        vkAttachment.push_back(((ImageData*)frameBuffer.mAttachments[i].mId)->view);
    }

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.attachmentCount = vkAttachment.size();
    createInfo.pAttachments = vkAttachment.data();
    createInfo.width = viewport.size.x;
    createInfo.height = viewport.size.y;
    createInfo.layers = 1;
    createInfo.renderPass = (VkRenderPass)renderPass.mId;

    VkFramebuffer fBuffer;
    vkCreateFramebuffer(mData->device, &createInfo, nullptr, &fBuffer);
    frameBuffer.mId = (uint64_t)fBuffer;
}

void Graphic::CreateDeviceImage(DeviceImage& image) 
{
    ImageData* data = new ImageData;
    image.mId = (uint64_t)data;

    VkImageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.arrayLayers = 1;
    createInfo.extent.width = image.GetSize().x;
    createInfo.extent.height = image.GetSize().y;
    createInfo.extent.depth = 1.f;
    createInfo.format = VulkanHelper::ConvertToVulkanFormat(image.GetFormat());
    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.mipLevels = 1;

    const std::pair<ImageUsage, VkImageUsageFlags> usageMap[] = {
        {ImageUsage::ColorOutput, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT},
        {ImageUsage::Sample, VK_IMAGE_USAGE_SAMPLED_BIT},
        {ImageUsage::Storage, VK_IMAGE_USAGE_STORAGE_BIT},
        {ImageUsage::SubpassInput, VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT},
        {ImageUsage::TransferDst, VK_IMAGE_USAGE_TRANSFER_DST_BIT},
        {ImageUsage::TransferSrc, VK_IMAGE_USAGE_TRANSFER_SRC_BIT},
        {ImageUsage::Depth, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT},
    };

    for(auto [usage, vkUsage] : usageMap)
    {
        if((image.mUsage & usage) != ImageUsage::None)
        {
            createInfo.usage |= vkUsage;
        }
    }

    vkCreateImage(mData->device, &createInfo, nullptr, &data->image);

    data->memory = VulkanHelper::AllocateMemoryForImage(mData->physicalDevice, mData->device, data->image, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


    VkImageViewCreateInfo viewCreateInfo = {};
    viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCreateInfo.image = data->image;
    viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewCreateInfo.format = createInfo.format;
    viewCreateInfo.subresourceRange.aspectMask = ((image.mUsage & ImageUsage::Depth) != ImageUsage::None) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT; 
    viewCreateInfo.subresourceRange.levelCount = 1;
    viewCreateInfo.subresourceRange.layerCount = 1;
    viewCreateInfo.subresourceRange.baseArrayLayer = 0;
    viewCreateInfo.subresourceRange.baseMipLevel = 0;

    vkCreateImageView(mData->device, &viewCreateInfo, nullptr, &data->view);

}

void Graphic::ExecuteCommandBuffer(CommandBuffer& commandBuffer, QueueType queueType) 
{

    VkCommandBuffer cmdBuffers[] = {(VkCommandBuffer)commandBuffer.mId};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBuffers;

    VkQueue queue;

    switch (queueType) 
    {
        case QueueType::Graphic:
            queue = mData->queues.graphic;
            break;
        case QueueType::Compute:
            queue = mData->queues.compute;
            break;
        case QueueType::Transfer:
            queue = mData->queues.transfer;
            break;
        case QueueType::Present:
            queue = mData->queues.present;
            break;
    }

    vkQueueSubmit(queue, 1, &submitInfo, nullptr);
}

void Graphic::WaitForDevice() 
{
    vkDeviceWaitIdle(mData->device);    
}


void Graphic::CreateSwapchain(Swapchain& swapchain) 
{
    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageExtent.width = swapchain.mSize.x;
    createInfo.imageExtent.height = swapchain.mSize.y;
    createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.minImageCount = 3;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.surface = mData->surface;

    VkSwapchainKHR swpc;
    vkCreateSwapchainKHR(mData->device, &createInfo, nullptr, &swpc);

    swapchain.mId = (uint64_t)swpc;

    uint32_t count;
    vkGetSwapchainImagesKHR(mData->device, swpc, &count, nullptr);
    std::vector<VkImage> images(count);
    vkGetSwapchainImagesKHR(mData->device, swpc, &count, images.data());

    for (size_t i = 0; i < images.size(); i++)
    {
        DeviceImage img;

        ImageData* data = new ImageData;

        VkImage image = images[i];


        img.mId = (uint64_t)data;

        VkImageViewCreateInfo viewCreateInfo = {};
        viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCreateInfo.image = image;
        viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewCreateInfo.format = createInfo.imageFormat;
        viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCreateInfo.subresourceRange.levelCount = 1;
        viewCreateInfo.subresourceRange.layerCount = 1;
        viewCreateInfo.subresourceRange.baseArrayLayer = 0;
        viewCreateInfo.subresourceRange.baseMipLevel = 0;

        VkImageView view;
        vkCreateImageView(mData->device, &viewCreateInfo, nullptr, &view);

        data->image = image;
        data->view = view;

        swapchain.mImages.push_back(img);
    }

}

void Graphic::CreateFrameBufferWithoutAttachments(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport) 
{
    
}
