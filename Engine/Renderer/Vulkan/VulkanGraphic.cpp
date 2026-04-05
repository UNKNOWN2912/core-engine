#include <Renderer/Graphic.hpp>
#include "Renderer/Vulkan/VulkanSwapchain.hpp"
#include <cstring>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Core/Macro.hpp>

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
}

void Graphic::DestroyVulkanObjects() 
{
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

    for (size_t i = 0; i < renderPass.GetAttachments().size(); i++)
    {
        const Attachment& attachment = renderPass.GetAttachments()[i];
        VkAttachmentDescription description = VulkanHelper::GetVulkanAttachmentFromAttachment(attachment);
        attachmentDescriptions.push_back(description); 
    }

    for (size_t i = 0; i < renderPass.GetSubpasses().size(); i++)
    {
        const Subpass& subpass = renderPass.GetSubpasses()[i];
        const Dependency& dependency = renderPass.GetDependencies()[i];

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
    
    renderPass.SetId((uint64_t)vkRenderPass);

    for (size_t i = 0; i < subpassDescriptions.size(); i++)
    {
        delete[] subpassDescriptions[i].pColorAttachments;
        delete[] subpassDescriptions[i].pInputAttachments;
        delete[] subpassDescriptions[i].pDepthStencilAttachment;
    }
}


void Graphic::BeginCommandBufferRecording(const CommandBuffer& commandBuffer) 
{
    WaitForDevice();
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
    for (size_t i = 0; i < renderPass.GetAttachments().size(); i++)
    {
        const Attachment& attachment = renderPass.GetAttachments()[i];
        if(attachment.GetLoadOperation() == LoadOperation::Clear)
        {
            glm::vec4 color = attachment.GetClearValue();
            clearColors[clearColorCount].color = {{color.r, color.g, color.b, color.a}};
            clearColorCount++;
        }
    }


    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.renderArea.extent = {viewport.GetSize().x, viewport.GetSize().y};
    beginInfo.renderArea.offset = {int32_t(viewport.GetPosition().x), int32_t(viewport.GetPosition().y)};
    beginInfo.clearValueCount = clearColorCount;
    beginInfo.pClearValues = clearColors;
    beginInfo.framebuffer = (VkFramebuffer)frameBuffer.GetId();
    beginInfo.renderPass = (VkRenderPass)renderPass.GetId();

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
    for (size_t i = 0; i < renderPass.GetAttachments().size(); i++)
    {
        const Attachment& attachment = renderPass.GetAttachments()[i];
        DeviceImage image;
        image.mFormat = attachment.GetFormat();
        image.mSize = viewport.GetSize();
        image.mUsage = attachment.GetImageUsage();

        CreateDeviceImage(image);

        frameBuffer.AddAttachment(image);
    }

    std::vector<VkImageView> vkAttachment;

    for (size_t i = 0; i < frameBuffer.GetAttachments().size(); i++)
    {
        vkAttachment.push_back(((ImageData*)frameBuffer.GetAttachments()[i].mId)->view);
    }

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.attachmentCount = vkAttachment.size();
    createInfo.pAttachments = vkAttachment.data();
    createInfo.width = viewport.GetSize().x;
    createInfo.height = viewport.GetSize().y;
    createInfo.layers = 1;
    createInfo.renderPass = (VkRenderPass)renderPass.GetId();

    VkFramebuffer fBuffer;
    vkCreateFramebuffer(mData->device, &createInfo, nullptr, &fBuffer);
    frameBuffer.SetId((uint64_t)fBuffer);
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

void Graphic::ExecuteCommandBuffer(const CommandBuffer& commandBuffer, QueueType queueType, const std::vector<DeviceSemaphore>& waitSemaphores, DeviceSemaphore signalSemaphore) 
{

    std::vector<VkSemaphore> waitSemaphoreHandles;
    for (size_t i = 0; i < waitSemaphores.size(); i++)
    {
        waitSemaphoreHandles.push_back((VkSemaphore)waitSemaphores[i].mId);
    }

    std::vector<VkPipelineStageFlags> waitStages;
    for (size_t i = 0; i < waitSemaphores.size(); i++)
    {
        waitStages.push_back(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    }

    std::vector<VkCommandBuffer> cmdBuffers = {(VkCommandBuffer)commandBuffer.mId};

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = cmdBuffers.data();
    submitInfo.waitSemaphoreCount = waitSemaphores.size();
    submitInfo.pWaitSemaphores = waitSemaphoreHandles.data();
    submitInfo.pWaitDstStageMask = waitStages.data();
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = (VkSemaphore*)signalSemaphore.mId;

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
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mData->physicalDevice, mData->surface, &capabilities);



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
    VkResult result = vkCreateSwapchainKHR(mData->device, &createInfo, nullptr, &swpc);
    if(result != VK_SUCCESS)
    {
        LOG("Error: {}", (uint32_t)result);
    }

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

void Graphic::CreateFrameBufferWithUserAttachments(FrameBuffer& frameBuffer, const RenderPass& renderPass, const Viewport& viewport, const std::vector<DeviceImage>& images) 
{
    for (size_t i = 0; i < images.size(); i++)
    {
        frameBuffer.AddAttachment(images[i]);
    }

    std::vector<VkImageView> vkAttachment;

    for (size_t i = 0; i < frameBuffer.GetAttachments().size(); i++)
    {
        vkAttachment.push_back(((ImageData*)frameBuffer.GetAttachments()[i].mId)->view);
    }

    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.attachmentCount = vkAttachment.size();
    createInfo.pAttachments = vkAttachment.data();
    createInfo.width = viewport.GetSize().x;
    createInfo.height = viewport.GetSize().y;
    createInfo.layers = 1;
    createInfo.renderPass = (VkRenderPass)renderPass.GetId();

    VkFramebuffer fBuffer;
    vkCreateFramebuffer(mData->device, &createInfo, nullptr, &fBuffer);
    frameBuffer.SetId((uint64_t)fBuffer);
}

uint32_t Graphic::GetNextSwapchainImage(const Swapchain& swapchain, DeviceSemaphore semaphore) 
{
    uint32_t index;
    vkAcquireNextImageKHR(mData->device, (VkSwapchainKHR)swapchain.mId, UINT64_MAX, (VkSemaphore)semaphore.GetId(), VK_NULL_HANDLE, &index);
    return index;
}

void Graphic::PresentSwapchainImage(const Swapchain& swapchain, uint32_t imageIndex, DeviceSemaphore waitSemaphore) 
{
    VkSwapchainKHR swapchains[] = {(VkSwapchainKHR)swapchain.GetId()};
    VkSemaphore waitSemaphores[] = {(VkSemaphore)waitSemaphore.GetId()};

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pSwapchains = swapchains;
    presentInfo.swapchainCount = 1;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = waitSemaphores;

    vkQueuePresentKHR(mData->queues.present, &presentInfo);
}

void Graphic::CreateSemaphore(DeviceSemaphore& semaphore)
{
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphoreHandle;
    vkCreateSemaphore(mData->device, &createInfo, nullptr, &semaphoreHandle);

    semaphore.mId = (uint64_t)semaphoreHandle;
}

void Graphic::CreateShader(Shader& shader, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = (const uint32_t*)code.data();

    VkShaderModule shaderModule;
    vkCreateShaderModule(mData->device, &createInfo, nullptr, &shaderModule);

    shader.SetId((uint64_t)shaderModule);
}

void Graphic::CreateShaderFromFile(Shader& shader, std::string_view path)
{
    FILE* file = fopen(path.data(), "rb");
    if(file == nullptr)
    {
        LOG("Failed to open shader file: {}", path.data());
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::vector<char> code(fileSize);
    fread(code.data(), 1, fileSize, file);
    fclose(file);

    CreateShader(shader, code);
}

void Graphic::CreatePipelineLayout(PipelineLayout& pipelineLayout) 
{
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    for (size_t i = 0; i < pipelineLayout.GetDescriptorInfos().size(); i++)
    {
        const DescriptorDescription& description = pipelineLayout.GetDescriptorInfos()[i];
        VkDescriptorSetLayoutBinding binding = {};
        binding.binding = description.binding;
        binding.descriptorCount = 1;
        binding.stageFlags = VulkanHelper::ConvertToVulkanShaderStage(description.stage);
        binding.descriptorType = VulkanHelper::ConvertToVulkanDescriptorType(description.type);

        bindings.push_back(binding);
    }

    VkDescriptorSetLayoutCreateInfo setLayoutCreateInfo = {};
    setLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutCreateInfo.bindingCount = bindings.size();
    setLayoutCreateInfo.pBindings = bindings.data();
    
    VkDescriptorSetLayout setLayout;
    vkCreateDescriptorSetLayout(mData->device, &setLayoutCreateInfo, nullptr, &setLayout);

    VkPipelineLayoutCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.setLayoutCount = 1;
    createInfo.pSetLayouts = &setLayout;

    VkPipelineLayout pipelineLayoutHandle;
    vkCreatePipelineLayout(mData->device, &createInfo, nullptr, &pipelineLayoutHandle);
    pipelineLayout.SetId((uint64_t)pipelineLayoutHandle);
}

void Graphic::CreateGraphicPipeline(GraphicPipeline& graphicPipeline, const RenderPass& renderPass, uint64_t subpassIndex, const Viewport& viewport) 
{
    VkPipelineShaderStageCreateInfo shaderStages[2] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = (VkShaderModule)graphicPipeline.GetShader().GetVertexShader().GetId();
    shaderStages[0].pName = "main";

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = (VkShaderModule)graphicPipeline.GetShader().GetFragmentShader().GetId();
    shaderStages[1].pName = "main";


    uint32_t colorAttachmentCount = renderPass.GetSubpasses()[subpassIndex].mColorAttachmentIndex.size();
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments(colorAttachmentCount);
    for (size_t i = 0; i < colorAttachmentCount; i++)
    {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachments.emplace_back(colorBlendAttachment);
    }

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.attachmentCount = colorAttachmentCount;
    colorBlendStateCreateInfo.pAttachments = colorBlendAttachments.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VulkanHelper::ConvertToVulkanPrimitiveTopology(graphicPipeline.GetPrimitiveType());
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VulkanHelper::ConvertToVulkanCullMode(graphicPipeline.GetCullMode());
    rasterizationStateCreateInfo.frontFace = VulkanHelper::ConvertToVulkanFrontFace(graphicPipeline.GetFrontFace());
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = 1.f;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    if(graphicPipeline.GetSampleCount() == 1)
    {
        multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    }
    else
    {
        multisampleStateCreateInfo.rasterizationSamples = VulkanHelper::ConvertToVulkanSampleCount(graphicPipeline.GetSampleCount());
        multisampleStateCreateInfo.sampleShadingEnable = VK_TRUE;
        multisampleStateCreateInfo.minSampleShading = 1.f;
    }

    VkViewport vkViewport = {};
    vkViewport.x = viewport.GetPosition().x;
    vkViewport.y = viewport.GetPosition().y;
    vkViewport.width = viewport.GetSize().x;
    vkViewport.height = viewport.GetSize().y;
    vkViewport.minDepth = 0.f;
    vkViewport.maxDepth = 1.f;

    VkRect2D scissor = {};
    scissor.offset = {int32_t(viewport.GetPosition().x), int32_t(viewport.GetPosition().y)};
    scissor.extent = {viewport.GetSize().x, viewport.GetSize().y};

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;
    viewportStateCreateInfo.pViewports = &vkViewport;
    viewportStateCreateInfo.pScissors = &scissor;

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = graphicPipeline.IsDepthEnabled() ? VK_TRUE : VK_FALSE;
    depthStencilStateCreateInfo.depthWriteEnable = graphicPipeline.IsDepthEnabled() ? VK_TRUE : VK_FALSE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

    VertexLayout vertexLayout = graphicPipeline.GetVertexLayout();

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    std::vector<VkVertexInputBindingDescription> bindingDescriptions;

    for (size_t i = 0; i < vertexLayout.GetAttributes().size(); i++)
    {
        const Attribute& attribute = vertexLayout.GetAttributes()[i];

        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.binding = attribute.binding;
        attributeDescription.location = attribute.location;
        attributeDescription.format = VulkanHelper::ConvertToVulkanVertexFormat(attribute.format);
        attributeDescription.offset = attribute.offset;

        attributeDescriptions.emplace_back(attributeDescription);
    }

    for (size_t i = 0; i < vertexLayout.GetBindings().size(); i++)
    {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = vertexLayout.GetBindings()[i].binding;
        bindingDescription.inputRate = (vertexLayout.GetBindings()[i].perInstance) ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescription.stride = vertexLayout.GetBindings()[i].stride;

        bindingDescriptions.emplace_back(bindingDescription);
    }


    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo = {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = bindingDescriptions.size();
    vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkGraphicsPipelineCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.layout = (VkPipelineLayout)graphicPipeline.GetShader().GetPipelineLayout().GetId();
    createInfo.stageCount = 2;
    createInfo.pStages = shaderStages;
    createInfo.renderPass = (VkRenderPass)renderPass.GetId();
    createInfo.subpass = subpassIndex;
    createInfo.pColorBlendState = &colorBlendStateCreateInfo;
    createInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    createInfo.pRasterizationState = &rasterizationStateCreateInfo;
    createInfo.pMultisampleState = &multisampleStateCreateInfo;
    createInfo.pViewportState = &viewportStateCreateInfo;
    createInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    createInfo.pVertexInputState = &vertexInputStateCreateInfo;

    VkPipeline graphicPipelineHandle;
    vkCreateGraphicsPipelines(mData->device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicPipelineHandle);

    graphicPipeline.SetId((uint64_t)graphicPipelineHandle);
}