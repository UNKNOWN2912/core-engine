#include "VulkanGraphicPipeline.hpp"
#include "VulkanUtility.hpp"

void VulkanGraphicPipeline::Create(VkDevice device, VkRenderPass renderPass, uint32_t subpass, VkPipelineLayout pipelineLayout)
{
    mDevice = device;

    vkAssert(mDevice);
    vkAssert(mVertexShaderModule);
    vkAssert(mFragmentShaderModule);

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = mVertexShaderModule;
    vertexShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = { VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = mFragmentShaderModule;
    fragmentShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = { vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo };

    VkViewport viewport = {};
    viewport.width = 800;
    viewport.height = 600;
    viewport.maxDepth = 1.f;
    viewport.minDepth = 1.f;
    VkRect2D scissor = {};
    scissor.extent = { 800, 600 };

    VkPipelineViewportStateCreateInfo viewportState = { VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
    viewportState.pViewports = &viewport;
    viewportState.viewportCount = 1;
    viewportState.pScissors = &scissor;
    viewportState.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo colorBlendState = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
    colorBlendState.attachmentCount = mColorBlendAttachments.size();
    colorBlendState.pAttachments = mColorBlendAttachments.data();

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicState = { VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = { VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineMultisampleStateCreateInfo multisample = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineRasterizationStateCreateInfo rasterization = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
    rasterization.cullMode = VK_CULL_MODE_NONE;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    if (mWireFrameEnabled)
        rasterization.polygonMode = VK_POLYGON_MODE_LINE;
    rasterization.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterization.lineWidth = 1.f;

    VkPipelineVertexInputStateCreateInfo vertexInput = { VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
    vertexInput.pVertexAttributeDescriptions = mAttributes.data();
    vertexInput.vertexAttributeDescriptionCount = mAttributes.size();
    vertexInput.pVertexBindingDescriptions = mBindings.data();
    vertexInput.vertexBindingDescriptionCount = mBindings.size();

    VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
    depthStencil.minDepthBounds = 0.f;
    depthStencil.maxDepthBounds = 1.f;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
    pipelineCreateInfo.layout = pipelineLayout;
    pipelineCreateInfo.renderPass = renderPass;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStageCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pMultisampleState = &multisample;
    pipelineCreateInfo.pRasterizationState = &rasterization;
    pipelineCreateInfo.pVertexInputState = &vertexInput;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.subpass = subpass;

    if (mDepthTestingEnabled)
        pipelineCreateInfo.pDepthStencilState = &depthStencil;

    vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &mHandle);
}

void VulkanGraphicPipeline::AddVertexAttribute(uint32_t binding, uint32_t location, uint32_t offset, VkFormat format)
{
    VkVertexInputAttributeDescription attributeDescription = {};
    attributeDescription.binding = binding;
    attributeDescription.location = location;
    attributeDescription.offset = offset;
    attributeDescription.format = format;

    mAttributes.push_back(attributeDescription);
}

void VulkanGraphicPipeline::AddVertexBinding(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = binding;
    bindingDescription.inputRate = inputRate;
    bindingDescription.stride = stride;

    mBindings.push_back(bindingDescription);
}

void VulkanGraphicPipeline::AddColorBlendAttachment()
{
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    mColorBlendAttachments.push_back(colorBlendAttachment);
}

VulkanGraphicPipeline::VulkanGraphicPipeline() : mAttributes(0), mBindings(0), mVertexShaderModule(VK_NULL_HANDLE), mFragmentShaderModule(VK_NULL_HANDLE), mHandle(VK_NULL_HANDLE), mDepthTestingEnabled(VK_FALSE), mWireFrameEnabled(VK_FALSE) {}

void VulkanGraphicPipeline::SetVertexShader(VkShaderModule shaderModule) { mVertexShaderModule = shaderModule; }

void VulkanGraphicPipeline::SetFragmentShader(VkShaderModule shaderModule) { mFragmentShaderModule = shaderModule; }

VkPipeline VulkanGraphicPipeline::GetHandle() { return mHandle; }

void VulkanGraphicPipeline::EnableWireFrame(VkBool32 enable) { mWireFrameEnabled = enable; }

void VulkanGraphicPipeline::EnableDepthTesting(VkBool32 enable) { mDepthTestingEnabled = enable; }
