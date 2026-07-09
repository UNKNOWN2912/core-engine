#include "GraphicsPipeline.hpp"
#include "Core/Macro.hpp"
#include "GraphicsContext.hpp"
#include "Renderer/Converter.hpp"
#include "Renderer/Descriptor.hpp"
#include "Utility.hpp"

void GraphicsPipeline::SetVertexShader(VkShaderModule shader)
{
    mVertexShader = shader;
}

void GraphicsPipeline::SetFragmentShader(VkShaderModule shader)
{
    mFragmentShader = shader;
}

void GraphicsPipeline::SetGeometryShader(VkShaderModule shader)
{
    mGeometryShader = shader;
}

void GraphicsPipeline::SetTessellationShader(VkShaderModule shader)
{
    mTessellationShader = shader;
}

void GraphicsPipeline::EnableDepthTesting(bool enable)
{
    CHROME_TRACE_FUNCTION();
    mDepthTestEnable = enable;
}

void GraphicsPipeline::EnableDepthWrite(bool enable)
{
    CHROME_TRACE_FUNCTION();
    mDepthWriteEnable = enable;
}

void GraphicsPipeline::EnableBlending(bool enable)
{
    CHROME_TRACE_FUNCTION();
    mBlendEnable = enable;
}

void GraphicsPipeline::EnableWireframe(bool enable)
{
    CHROME_TRACE_FUNCTION();
    mWireframeEnable = enable;
}

void GraphicsPipeline::AddBinding(uint32_t binding, size_t stride, InputRate inputRate)
{
    CHROME_TRACE_FUNCTION();
    VkVertexInputBindingDescription description =
        {
            .binding = binding,
            .stride = (uint32_t)stride,
            .inputRate = GetVulkanInputRate(inputRate),
        };

    mBindingDescription.push_back(description);
}

void GraphicsPipeline::AddAttribute(uint32_t binding, uint32_t location, ImageFormat format, size_t offset)
{
    CHROME_TRACE_FUNCTION();
    VkVertexInputAttributeDescription description =
        {
            .location = location,
            .binding = binding,
            .format = GetVulkanImageFormat(format),
            .offset = (uint32_t)offset};

    mAttributeDescription.push_back(description);
}

void GraphicsPipeline::SetCullMode(CullMode cullMode)
{
    CHROME_TRACE_FUNCTION();
    mCullMode = GetVulkanCullMode(cullMode);
}
void GraphicsPipeline::SetPrimitive(PrimitiveType primitive)
{
    CHROME_TRACE_FUNCTION();
    mPrimitive = GetVulkanPrimitive(primitive);
}

void GraphicsPipeline::SetMultisampleCount(SampleCount count)
{
    CHROME_TRACE_FUNCTION();
    mSampleCount = GetVulkanSampleCount(count);
}

void GraphicsPipeline::SetFrontFace(FrontFace frontFace)
{
    mFrontFace = GetVulkanFrontsFace(frontFace);
}

void GraphicsPipeline::SetViewport(const VkViewport &viewport)
{
    CHROME_TRACE_FUNCTION();
    mViewport = viewport;
}

void GraphicsPipeline::CmdBindPipeline(const CommandBuffer &commandBuffer) const
{
    vkCmdBindPipeline(commandBuffer.GetHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, mHandle);
}

void GraphicsPipeline::AddColorBlendAttachment(bool enableBlending)
{
    CHROME_TRACE_FUNCTION();
    VkPipelineColorBlendAttachmentState state =
        {
            .blendEnable = VK_TRUE,
            .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
            .colorBlendOp = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp = VK_BLEND_OP_ADD,
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };

    mColorBlendStates.push_back(state);
}

void GraphicsPipeline::CreatePipeline(const RenderPass &renderPass, uint32_t subpassIndex)
{
    CHROME_TRACE_FUNCTION();
    if (mVertexShader == VK_NULL_HANDLE || mFragmentShader == VK_NULL_HANDLE)
    {
        ERROR("Graphic pipeline: vertex and fragment shader must be provided");
        return;
    }

    VkPipelineShaderStageCreateInfo vertexShaderStageCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    vertexShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageCreateInfo.module = mVertexShader;
    vertexShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    fragmentShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageCreateInfo.module = mFragmentShader;
    fragmentShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo geometryShaderStageCreateInfo = {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO};
    geometryShaderStageCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    geometryShaderStageCreateInfo.module = mGeometryShader;
    geometryShaderStageCreateInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[] = {vertexShaderStageCreateInfo, fragmentShaderStageCreateInfo, geometryShaderStageCreateInfo};

    VkViewport viewport = {};
    viewport.width = 800;
    viewport.height = 600;
    viewport.maxDepth = 1.f;
    viewport.minDepth = 0.f;
    VkRect2D scissor = {};
    scissor.extent = {800, 600};

    VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.pViewports = &viewport;
    viewportState.viewportCount = 1;
    viewportState.pScissors = &scissor;
    viewportState.scissorCount = 1;

    VkPipelineColorBlendStateCreateInfo colorBlendState = {VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO};
    colorBlendState.attachmentCount = mColorBlendStates.size();
    colorBlendState.pAttachments = mColorBlendStates.data();
    colorBlendState.logicOpEnable = VK_FALSE;
    colorBlendState.logicOp = VK_LOGIC_OP_COPY;

    VkDynamicState dynamicStates[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, VK_DYNAMIC_STATE_CULL_MODE, VK_DYNAMIC_STATE_DEPTH_WRITE_ENABLE, VK_DYNAMIC_STATE_DEPTH_TEST_ENABLE};

    VkPipelineDynamicStateCreateInfo dynamicState = {VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
    dynamicState.dynamicStateCount = sizeof(dynamicStates) / sizeof(VkDynamicState);
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineMultisampleStateCreateInfo multisample = {VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};
    multisample.rasterizationSamples = mSampleCount;

    VkPipelineRasterizationStateCreateInfo rasterization = {VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};
    rasterization.cullMode = mCullMode;
    rasterization.polygonMode = VK_POLYGON_MODE_FILL;
    if (mWireframeEnable)
        rasterization.polygonMode = VK_POLYGON_MODE_LINE;
    rasterization.frontFace = mFrontFace;
    rasterization.lineWidth = 1.f;

    VkPipelineVertexInputStateCreateInfo vertexInput = {VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO};
    vertexInput.pVertexAttributeDescriptions = mAttributeDescription.data();
    vertexInput.vertexAttributeDescriptionCount = mAttributeDescription.size();
    vertexInput.pVertexBindingDescriptions = mBindingDescription.data();
    vertexInput.vertexBindingDescriptionCount = mBindingDescription.size();

    VkPipelineDepthStencilStateCreateInfo depthStencil = {VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};
    depthStencil.minDepthBounds = 0.f;
    depthStencil.maxDepthBounds = 1.f;
    depthStencil.depthWriteEnable = mDepthWriteEnable;
    depthStencil.depthTestEnable = mDepthTestEnable;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;

    std::vector<VkPushConstantRange> ranges;
    for (auto &[stage, range] : mPushConstants)
    {
        ranges.push_back(range);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = (uint32_t)mSetLayouts.size(),
            .pSetLayouts = mSetLayouts.data(),
            .pushConstantRangeCount = (uint32_t)ranges.size(),
            .pPushConstantRanges = ranges.data(),
        };

    vkCreatePipelineLayout(GraphicsContext::GetDevice(), &pipelineLayoutCreateInfo, nullptr, &mPipelineLayout);

    uint32_t stageCount = 2;
    if (mGeometryShader != VK_NULL_HANDLE)
    {
        stageCount = 3;
    }

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO};
    pipelineCreateInfo.layout = mPipelineLayout;
    pipelineCreateInfo.renderPass = renderPass.GetHandle();
    pipelineCreateInfo.stageCount = stageCount;
    pipelineCreateInfo.pStages = shaderStageCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
    pipelineCreateInfo.pMultisampleState = &multisample;
    pipelineCreateInfo.pRasterizationState = &rasterization;
    pipelineCreateInfo.pVertexInputState = &vertexInput;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.subpass = subpassIndex;
    pipelineCreateInfo.pDepthStencilState = &depthStencil;

    vkCreateGraphicsPipelines(GraphicsContext::GetDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &mHandle);
}

void GraphicsPipeline::DestroyPipeline()
{
    vkDestroyPipeline(GraphicsContext::GetDevice(), mHandle, nullptr);
}

VkPipelineLayout GraphicsPipeline::GetPipelineLayout() const
{
    CHROME_TRACE_FUNCTION();
    return mPipelineLayout;
}

VkPipeline GraphicsPipeline::GetHandle() const
{
    return mHandle;
}

void GraphicsPipeline::SetPushConstant(ShaderStage stage, size_t size)
{
    VkPushConstantRange range =
        {
            .stageFlags = GetVulkanShaderStage(stage),
            .offset = 0,
            .size = uint32_t(size),
        };

    mPushConstants[stage] = range;
}

void GraphicsPipeline::ClearAttributesAndBinding()
{
    mAttributeDescription.clear();
    mBindingDescription.clear();
}
