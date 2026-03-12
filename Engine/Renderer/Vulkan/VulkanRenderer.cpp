#include "Renderer/Vulkan/VulkanGraphicPipeline.hpp"
#include <Renderer/Vulkan/VulkanRenderer.hpp>
#include <Renderer/Vulkan/VulkanUtility.hpp>
#include <cassert>
#include <print>
#include <Core/Macro.hpp>


Renderer* Renderer::Create(const Window& window)
{
	return new VulkanRenderer(window);
}

VulkanRenderer::VulkanRenderer(const Window& window)
{
	Initialize(window);
}


void VulkanRenderer::Initialize(const Window& window)
{
	LOG("Vulkan renderer initialization");

	CreateVulkanObjects(window);
}

void VulkanRenderer::CreateVulkanObjects(const Window& window) 
{
    mViewport.width = window.GetSize().x;
    mViewport.height = window.GetSize().y;
    mViewport.maxDepth = 1.f;
    mViewport.minDepth = 0.f;

	CreateInstance();
    GetPhysicalDevice();
    CreateSurface(window);
    GetQueueIndices();
    CreateDevice();
    CreateQueues();
    CreateSwapchain(window);
    CreateCommandPool();
    CreatePipelineLayout();
    CreateRenderCommandBuffer();
    CreateMainRenderPass();
    CreateMainRenderPassFrameBuffer();
}

void VulkanRenderer::CreateCommandPool()
{
    vkAssert(mDevice);

    VkCommandPoolCreateInfo createInfo = {VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO};
    
    mCommandPool = createCommandPool(mDevice);
}

void VulkanRenderer::CreateMeshRendererPipeline(std::shared_ptr<MeshRenderer> meshRenderer) 
{
    LOG("Creating Mesh Renderer Pipeline");
    vkAssert(mDevice);
    vkAssert(mPipelineLayout);

    assert(mShaders.find(meshRenderer->GetMaterial()->shader) != mShaders.end());

    std::shared_ptr<VulkanGraphicPipeline> pipeline = std::make_shared<VulkanGraphicPipeline>();

    pipeline->SetVertexShader(mShaders[meshRenderer->GetMaterial()->shader].mVertexShader);
    pipeline->SetFragmentShader(mShaders[meshRenderer->GetMaterial()->shader].mFragmentShader);
    pipeline->AddColorBlendAttachment();

    pipeline->Create(mDevice, mMainRenderPass, 0, mPipelineLayout);

    mMeshRendererPipeline[meshRenderer] = pipeline;
}


void VulkanRenderer::CreateRenderCommandBuffer()
{
    LOG("Creating Render Command Buffer");
    vkAssert(mDevice);
    vkAssert(mCommandPool);
    mCommandBuffer = allocateCommandBuffers(mDevice, mCommandPool);
}

void VulkanRenderer::CreateInstance() 
{
	LOG("Creating Vulkan Instance");

	VkApplicationInfo appInfo = {VK_STRUCTURE_TYPE_APPLICATION_INFO};
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pApplicationName = "test";
	appInfo.apiVersion = VK_API_VERSION_1_4;
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "test";

	VkInstanceCreateInfo createInfo = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO};

	createInfo.pApplicationInfo = &appInfo;

	uint32_t extensionCount = 0;
	const char **extensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	createInfo.enabledExtensionCount = extensionCount;
	createInfo.ppEnabledExtensionNames = extensions;

	// #if VULKAN_VALIDATION_LAYER_ENABLEMENT

	const char *layers = {"VK_LAYER_KHRONOS_validation"};

	createInfo.enabledLayerCount = 1;
	createInfo.ppEnabledLayerNames = &layers;

	// #endif

	vkCreateInstance(&createInfo, nullptr, &mInstance);
}

void VulkanRenderer::GetPhysicalDevice()
{
	LOG("Getting Physical Device");
	vkAssert(mInstance);
	
	uint32_t count;

    vkEnumeratePhysicalDevices(mInstance, &count, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(count);
    vkEnumeratePhysicalDevices(mInstance, &count, physicalDevices.data());

    VkPhysicalDeviceProperties properties;

    for (VkPhysicalDevice physicalDevice : physicalDevices)
    {
        vkGetPhysicalDeviceProperties(physicalDevice, &properties);

        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            mPhysicalDevice = physicalDevice;
        }
    }

    
}

void VulkanRenderer::CreateDevice()
{
	LOG("Creating Vulkan Device");
	vkAssert(mPhysicalDevice);

    VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO};

    const char *extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = &extension;

    std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos;

    float priority = 1.f;

    VkDeviceQueueCreateInfo graphicQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    graphicQueueCreateInfo.queueCount = 1;
    graphicQueueCreateInfo.pQueuePriorities = &priority;
    graphicQueueCreateInfo.queueFamilyIndex = mQueueIndices.graphic;

    VkDeviceQueueCreateInfo presentQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    presentQueueCreateInfo.queueCount = 1;
    presentQueueCreateInfo.pQueuePriorities = &priority;
    presentQueueCreateInfo.queueFamilyIndex = mQueueIndices.present;

    VkDeviceQueueCreateInfo computeQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    computeQueueCreateInfo.queueCount = 1;
    computeQueueCreateInfo.pQueuePriorities = &priority;
    computeQueueCreateInfo.queueFamilyIndex = mQueueIndices.compute;

    VkDeviceQueueCreateInfo transferQueueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
    transferQueueCreateInfo.queueCount = 1;
    transferQueueCreateInfo.pQueuePriorities = &priority;
    transferQueueCreateInfo.queueFamilyIndex = mQueueIndices.transfer;

    queuesCreateInfos.push_back(graphicQueueCreateInfo);
    if (mQueueIndices.present != mQueueIndices.graphic && mQueueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(presentQueueCreateInfo);
    }

    if (mQueueIndices.compute != mQueueIndices.present && mQueueIndices.compute != mQueueIndices.graphic && mQueueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(computeQueueCreateInfo);
    }

    if (mQueueIndices.transfer != mQueueIndices.present && mQueueIndices.transfer != mQueueIndices.graphic && mQueueIndices.transfer != mQueueIndices.compute && mQueueIndices.present != UINT32_MAX)
    {
        queuesCreateInfos.push_back(transferQueueCreateInfo);
    }

    createInfo.pQueueCreateInfos = queuesCreateInfos.data();
    createInfo.queueCreateInfoCount = queuesCreateInfos.size();

    vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice);
}

void VulkanRenderer::GetQueueIndices()
{
	LOG("Getting Queue Indices");
	vkAssert(mPhysicalDevice);
	vkAssert(mSurface);

    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(count);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &count, properties.data());

    QueueIndex queueIndices;

    for (int i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && queueIndices.graphic == UINT32_MAX)
        {
            queueIndices.graphic = i;
        }
        if (properties[i].queueFlags & VK_QUEUE_TRANSFER_BIT && queueIndices.compute == UINT32_MAX)
        {
            queueIndices.transfer = i;
        }
        if (properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT && queueIndices.compute == UINT32_MAX)
        {
            queueIndices.compute = i;
        }
        VkBool32 supported;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, i, mSurface, &supported);
        if (supported == VK_TRUE && queueIndices.present == UINT32_MAX)
        {
            queueIndices.present = i;
        }
    }

    mQueueIndices = queueIndices;
}

void VulkanRenderer::CreateQueues()
{
	LOG("Creating Device Queue");
    vkAssert(mDevice);

    Queue queues;

    vkGetDeviceQueue(mDevice, mQueueIndices.graphic, 0, &queues.graphic);
    vkGetDeviceQueue(mDevice, mQueueIndices.present, 0, &queues.present);
    vkGetDeviceQueue(mDevice, mQueueIndices.compute, 0, &queues.compute);
    vkGetDeviceQueue(mDevice, mQueueIndices.transfer, 0, &queues.transfer);

    mQueues = queues;
}

void VulkanRenderer::CreateSurface(const Window& window) 
{
    LOG("Creating Window Surface");
    vkAssert(mInstance);
    glfwCreateWindowSurface(mInstance, (GLFWwindow*)window.GetNativeWindow(), nullptr, &mSurface);
}

void VulkanRenderer::CreateMainRenderPass() 
{
    LOG("Creating Main Render Pass");

    vkAssert(mDevice);

    VkAttachmentDescription swapchainAttachment = populateAttachmentDescription(
        VK_FORMAT_B8G8R8A8_SRGB, 
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ATTACHMENT_LOAD_OP_CLEAR, 
        VK_ATTACHMENT_STORE_OP_STORE, 
        VK_ATTACHMENT_LOAD_OP_DONT_CARE, 
        VK_ATTACHMENT_STORE_OP_DONT_CARE, 
        VK_SAMPLE_COUNT_1_BIT
    );
    
    VkAttachmentReference swapchainAttachmentRef = populateAttachmentReference(0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
    
    VkSubpassDescription colorSubpass = {};
    colorSubpass.colorAttachmentCount = 1;
    colorSubpass.pColorAttachments = &swapchainAttachmentRef;
    colorSubpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    VkSubpassDependency colorDependency = {};
    colorDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    colorDependency.dstSubpass = 0;
    colorDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    colorDependency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    colorDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    VkRenderPassCreateInfo createInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO};
    createInfo.pSubpasses = &colorSubpass;
    createInfo.subpassCount = 1;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &swapchainAttachment;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &colorDependency;

    vkCreateRenderPass(mDevice, &createInfo, nullptr, &mMainRenderPass);
}

void VulkanRenderer::CreateMainRenderPassFrameBuffer() 
{
    LOG("Creating Framebuffer");

    vkAssert(mMainRenderPass);
    
    for (int i = 0; i < mSwapchain.GetImageCount(); i++)
    {
        VkFramebuffer frameBuffer = createFramebuffer(mDevice, mMainRenderPass, {uint32_t(mViewport.width), uint32_t(mViewport.height)}, {mSwapchain.GetImageViews()[i]});
        mFrameData.push_back({frameBuffer});
    }
}

void VulkanRenderer::CreateSwapchain(const Window& window) 
{
    LOG("Creating Swapchain");

    vkAssert(mPhysicalDevice);
    vkAssert(mDevice);
    vkAssert(mSurface);

    mSwapchain.Create({window.GetSize().x, window.GetSize().y}, mPhysicalDevice, mDevice, mSurface);

    mImageAcquiredSemaphore = createSemaphore(mDevice);
    mRenderFinished = createSemaphore(mDevice);
}




void VulkanRenderer::BeginFrame(const Vector4f& clearColor)
{
    mClearValue.color = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
}

void VulkanRenderer::EndFrame()
{
    vkDeviceWaitIdle(mDevice);

    uint32_t imageIndex = mSwapchain.GetImageIndex(mImageAcquiredSemaphore);

    VkCommandBufferBeginInfo beginInfo = {VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    vkBeginCommandBuffer(mCommandBuffer, &beginInfo);

    VkClearValue clearColor = mClearValue;

    VkRenderPassBeginInfo renderPassBeginInfo = {VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    renderPassBeginInfo.renderPass = mMainRenderPass;
    renderPassBeginInfo.renderArea.extent = {uint32_t(mViewport.width), uint32_t(mViewport.height)};
    renderPassBeginInfo.clearValueCount = 1;
    renderPassBeginInfo.pClearValues = &clearColor;
    renderPassBeginInfo.framebuffer = mFrameData[imageIndex].frameBuffer;
    vkCmdBeginRenderPass(mCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);


    VkRect2D scissor = {};
    scissor.extent = {uint32_t(mViewport.width), uint32_t(mViewport.height)};

    vkCmdSetViewport(mCommandBuffer, 0, 1, &mViewport);
    vkCmdSetScissor(mCommandBuffer, 0, 1, &scissor);


    for (int i = 0; i < mMeshRenderers.size(); i++)
    {
        vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, 
            mMeshRendererPipeline[mMeshRenderers[i]]->GetHandle());

        vkCmdDraw(mCommandBuffer, 3, 1, 0, 0);
    }


    vkCmdEndRenderPass(mCommandBuffer);

    vkEndCommandBuffer(mCommandBuffer);

    submitCommandBuffer(mQueues.graphic, mCommandBuffer, mImageAcquiredSemaphore, 
        mRenderFinished, VK_NULL_HANDLE, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);


    VkSwapchainKHR swapchain[] = {mSwapchain.GetHandle()};

    VkPresentInfoKHR presentInfo = {VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pSwapchains = swapchain;
    presentInfo.swapchainCount = 1;
    presentInfo.pWaitSemaphores = &mRenderFinished;
    presentInfo.waitSemaphoreCount = 1;
    vkQueuePresentKHR(mQueues.present, &presentInfo);

    mMeshRenderers.clear();
}

void VulkanRenderer::Submit(std::shared_ptr<MeshRenderer> meshRenderer)
{
    mMeshRenderers.push_back(meshRenderer);
}

void VulkanRenderer::CreateAssets(std::shared_ptr<AssetManager> assetManager) 
{
    const std::unordered_map<std::string, std::shared_ptr<Shader>>& map = assetManager->GetShaderMap();

    for(const auto& pair : map)
    {
        VulkanShader vkShader;
        std::string str = pair.first;
        std::shared_ptr<Shader> shader = pair.second;
        
        VkShaderModuleCreateInfo vertexShaderCreateInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};         
        vertexShaderCreateInfo.codeSize = shader->vertexCode.size();
        vertexShaderCreateInfo.pCode = (uint32_t*)shader->vertexCode.data();

        VkShaderModuleCreateInfo fragmentShaderCreateInfo = {VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO};         
        fragmentShaderCreateInfo.codeSize = shader->fragmentCode.size();
        fragmentShaderCreateInfo.pCode = (uint32_t*)shader->fragmentCode.data();

        VkShaderModule vertexShaderModule;
        VkShaderModule fragmentShaderModule;

        vkCreateShaderModule(mDevice, &vertexShaderCreateInfo, nullptr, &vertexShaderModule);
        vkCreateShaderModule(mDevice, &fragmentShaderCreateInfo, nullptr, &fragmentShaderModule);

        vkShader.mVertexShader = vertexShaderModule;
        vkShader.mFragmentShader = fragmentShaderModule;

        mShaders[str] = vkShader;
    }

}

void VulkanRenderer::CreateMeshRendererObjects(std::shared_ptr<MeshRenderer> meshRenderer) 
{
    CreateMeshRendererPipeline(meshRenderer);
}

void VulkanRenderer::CreatePipelineLayout() 
{
    vkAssert(mDevice);
    mPipelineLayout = createPipelineLayout(mDevice, {}, {});
}
