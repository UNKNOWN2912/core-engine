#include "Renderer/Converter.hpp"
#include "Core/Macro.hpp"

VkFormat GetVulkanImageFormat(ImageFormat imageFormat)
{
    switch (imageFormat) 
    {
        case ImageFormat::None:
            return VK_FORMAT_UNDEFINED;
        case ImageFormat::R8:
            return VK_FORMAT_R8_SRGB;
        case ImageFormat::RG8:
            return VK_FORMAT_R8G8_SRGB;
        case ImageFormat::RGB8:
            return VK_FORMAT_R8G8B8_SRGB;
        case ImageFormat::RGBA8:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat::R16:
            return VK_FORMAT_R16_SFLOAT;
        case ImageFormat::RG16:
            return VK_FORMAT_R16G16_SFLOAT;
        case ImageFormat::RGB16:
            return VK_FORMAT_R16G16B16_SFLOAT;
        case ImageFormat::RGBA16:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::R16U:
            return VK_FORMAT_R16_UINT;
        case ImageFormat::RG16U:
            return VK_FORMAT_R16G16_UINT;
        case ImageFormat::RGB16U:
            return VK_FORMAT_R16G16B16_UINT;
        case ImageFormat::RGBA16U:
            return VK_FORMAT_R16G16B16A16_UINT;
        case ImageFormat::R32:
            return VK_FORMAT_R32_SFLOAT;
        case ImageFormat::RG32:
            return VK_FORMAT_R32G32_SFLOAT;
        case ImageFormat::RGB32:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case ImageFormat::RGBA32:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ImageFormat::R32U:
            return VK_FORMAT_R32_UINT;
        case ImageFormat::RG32U:
            return VK_FORMAT_R32G32_UINT;
        case ImageFormat::RGB32U:
            return VK_FORMAT_R32G32B32_UINT;
        case ImageFormat::RGBA32U:
            return VK_FORMAT_R32G32B32A32_UINT;
        case ImageFormat::R64:
            return VK_FORMAT_R64_SFLOAT;
        case ImageFormat::RG64:
            return VK_FORMAT_R64G64_SFLOAT;
        case ImageFormat::RGB64:
            return VK_FORMAT_R64G64B64_SFLOAT;
        case ImageFormat::RGBA64:
            return VK_FORMAT_R64G64B64A64_SFLOAT;
        case ImageFormat::R64U:
            return VK_FORMAT_R64_UINT;
        case ImageFormat::RG64U:
            return VK_FORMAT_R64G64_UINT;
        case ImageFormat::RGB64U:
            return VK_FORMAT_R64G64B64_UINT;
        case ImageFormat::RGBA64U:
            return VK_FORMAT_R64G64B64A64_UINT;
        case ImageFormat::D32:
            return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::D24_S8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case ImageFormat::BGRA8:
            return VK_FORMAT_B8G8R8A8_SRGB;
    }

    ERROR("Invalid format: {}", (uint32_t)imageFormat);

    return VK_FORMAT_UNDEFINED;
}

VkImageLayout GetVulkanImageLayout(ImageLayout imageLayout)
{
    switch (imageLayout) 
    {
        case ImageLayout::None:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case ImageLayout::Color:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        case ImageLayout::DepthStencil:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case ImageLayout::ShaderRead:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case ImageLayout::TransferSource:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case ImageLayout::TransferDestination:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case ImageLayout::PresentSource:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    ERROR("Invalid image layout: {}", (uint32_t)imageLayout);

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageUsageFlags GetVulkanImageUsage(ImageUsage imageUsage)
{
    VkImageUsageFlags usage = 0;

    if((ImageUsage::None & imageUsage) == ImageUsage::None)
        usage |= 0;

    if((ImageUsage::Color & imageUsage) == ImageUsage::Color)
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if((ImageUsage::DepthStencil & imageUsage) == ImageUsage::DepthStencil)
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if((ImageUsage::Sampler & imageUsage) == ImageUsage::Sampler)
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;

    if((ImageUsage::TransferSource & imageUsage) == ImageUsage::TransferSource)
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if((ImageUsage::TransferDestination & imageUsage) == ImageUsage::TransferDestination)
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    if((ImageUsage::Storage & imageUsage) == ImageUsage::Storage)
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;


    return usage;
}

VkBufferUsageFlags GetVulkanBufferUsage(BufferUsage bufferUsage)
{
    VkBufferUsageFlags usage;

    if((BufferUsage::None & bufferUsage) == BufferUsage::None)
        usage |= 0;

    if((BufferUsage::VertexBuffer & bufferUsage) == BufferUsage::VertexBuffer)
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    if((BufferUsage::IndexBuffer & bufferUsage) == BufferUsage::IndexBuffer)
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    if((BufferUsage::UniformBuffer & bufferUsage) == BufferUsage::UniformBuffer)
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    if((BufferUsage::TransferSource & bufferUsage) == BufferUsage::TransferSource)
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    if((BufferUsage::TransferDestination & bufferUsage) == BufferUsage::TransferDestination)
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if((BufferUsage::Storage & bufferUsage) == BufferUsage::Storage)
        usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;


    return usage;
}

VkImageAspectFlags GetVulkanImageAspect(ImageAspect imageAspect)
{
    switch (imageAspect)
    {
        case ImageAspect::None:
            return VK_IMAGE_ASPECT_NONE;
        case ImageAspect::Color:
            return VK_IMAGE_ASPECT_COLOR_BIT;
        case ImageAspect::Depth:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
    };

    ERROR("Invalid image aspect: {}", (uint32_t)imageAspect);

    return 0;
}

VkAttachmentLoadOp GetVulkanLoadOperation(LoadOperation loadOperation)
{
    switch (loadOperation)
    {
        case LoadOperation::None:
            return VK_ATTACHMENT_LOAD_OP_NONE;
        case LoadOperation::Load:
            return VK_ATTACHMENT_LOAD_OP_LOAD;
        case LoadOperation::Clear:
            return VK_ATTACHMENT_LOAD_OP_CLEAR;
        case LoadOperation::DontCare:
            return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
    ERROR("Invalid load operation: {}", (uint32_t)loadOperation);

    return VK_ATTACHMENT_LOAD_OP_NONE;
}

VkAttachmentStoreOp GetVulkanStoreOperation(StoreOperation storeOperation)
{
    switch (storeOperation)
    {
        case StoreOperation::None:
            return VK_ATTACHMENT_STORE_OP_NONE;
        case StoreOperation::Store:
            return VK_ATTACHMENT_STORE_OP_STORE;
        case StoreOperation::DontCare:
            return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    }
    ERROR("Invalid store operation: {}", (uint32_t)storeOperation);

    return VK_ATTACHMENT_STORE_OP_NONE;
}

VkDescriptorType GetVulkanDescriptorType(DescriptorType descriptorType)
{
    switch (descriptorType)
    {
        case DescriptorType::None:
            return VK_DESCRIPTOR_TYPE_MAX_ENUM;
        case DescriptorType::Uniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case DescriptorType::CombinedSampler:
            return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        case DescriptorType::InputAttachment:
            return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    }
    ERROR("Invalid descriptor type: {}", (uint32_t)descriptorType);

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkPipelineStageFlags GetVulkanPipelineStage(PipelineStage pipelineStage)
{
    VkPipelineStageFlags stage = 0;

    if((PipelineStage::None & pipelineStage) == PipelineStage::None)
        stage |= VK_PIPELINE_STAGE_NONE;

    if((PipelineStage::TopOfPipe & pipelineStage) == PipelineStage::TopOfPipe)
        stage |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    if((PipelineStage::DrawIndirect & pipelineStage) == PipelineStage::DrawIndirect)
        stage |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

    if((PipelineStage::VertexInput & pipelineStage) == PipelineStage::VertexInput)
        stage |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

    if((PipelineStage::VertexShader & pipelineStage) == PipelineStage::VertexShader)
        stage |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

    if((PipelineStage::TessellationControlShader & pipelineStage) == PipelineStage::TessellationControlShader)
        stage |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;

    if((PipelineStage::TessellationEvaluationShader & pipelineStage) == PipelineStage::TessellationEvaluationShader)
        stage |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;

    if((PipelineStage::GeometryShader & pipelineStage) == PipelineStage::GeometryShader)
        stage |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;

    if((PipelineStage::FragmentShader & pipelineStage) == PipelineStage::FragmentShader)
        stage |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

    if((PipelineStage::EarlyFragmentTests & pipelineStage) == PipelineStage::EarlyFragmentTests)
        stage |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    if((PipelineStage::LateFragmentTests & pipelineStage) == PipelineStage::LateFragmentTests)
        stage |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

    if((PipelineStage::ColorAttachmentOutput & pipelineStage) == PipelineStage::ColorAttachmentOutput)
        stage |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    if((PipelineStage::ComputeShader & pipelineStage) == PipelineStage::ComputeShader)
        stage |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    if((PipelineStage::Transfer & pipelineStage) == PipelineStage::Transfer)
        stage |= VK_PIPELINE_STAGE_TRANSFER_BIT;

    if((PipelineStage::BottomOfPipe & pipelineStage) == PipelineStage::BottomOfPipe)
        stage |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    if((PipelineStage::Host & pipelineStage) == PipelineStage::Host)
        stage |= VK_PIPELINE_STAGE_HOST_BIT;

    if((PipelineStage::AllGraphics & pipelineStage) == PipelineStage::AllGraphics)
        stage |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;

    if((PipelineStage::AllCommands & pipelineStage) == PipelineStage::AllCommands)
        stage |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;


    return stage;
}

VkShaderStageFlags GetVulkanShaderStage(ShaderStage shaderStage)
{
    switch (shaderStage)
    {
        case ShaderStage::None:
            return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        case ShaderStage::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::Geometry:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::Tessellation:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
    }
    ERROR("Invalid shader stage: {}", (uint32_t)shaderStage);

    return 0;
}

VkSampleCountFlagBits GetVulkanSampleCount(SampleCount sampleCount)
{
    switch (sampleCount)
    {
        case SampleCount::One:
            return VK_SAMPLE_COUNT_1_BIT;
        case SampleCount::Two:
            return VK_SAMPLE_COUNT_2_BIT;
        case SampleCount::Four:
            return VK_SAMPLE_COUNT_4_BIT;
        case SampleCount::Eight:
            return VK_SAMPLE_COUNT_8_BIT;
        case SampleCount::Sixteen:
            return VK_SAMPLE_COUNT_16_BIT;
        case SampleCount::ThirtyTwo:
            return VK_SAMPLE_COUNT_32_BIT;
        case SampleCount::SixtyFour:
            return VK_SAMPLE_COUNT_64_BIT;
    }
    ERROR("Invalid sample count: {}", (uint32_t)sampleCount);

    return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
}

VkMemoryPropertyFlags GetVulkanMemoryProperty(MemoryProperty memoryProperty)
{
    switch (memoryProperty)
    {
        case MemoryProperty::None:
            return  VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
        case MemoryProperty::DeviceLocal:
            return  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        case MemoryProperty::HostVisible:
            return  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        case MemoryProperty::HostCoherent:
            return  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }
    ERROR("Invalid memory property: {}", (uint32_t)memoryProperty);

    return VK_MEMORY_PROPERTY_FLAG_BITS_MAX_ENUM;
}

VkPhysicalDeviceType GetVulkanDeviceType(DeviceType deviceType)
{
    switch (deviceType)
    {
        case DeviceType::None:
            return VK_PHYSICAL_DEVICE_TYPE_OTHER;
        case DeviceType::Dedicated:
            return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        case DeviceType::Integrated:
            return VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
        case DeviceType::VirtualGpu:
            return VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
        case DeviceType::Cpu:
            return VK_PHYSICAL_DEVICE_TYPE_CPU;
    }
    ERROR("Invalid device type: {}", (uint32_t)deviceType);

    return VK_PHYSICAL_DEVICE_TYPE_OTHER;
}

VkFilter GetVulkanFilter(Filter filter)
{
    switch (filter)
    {
        case Filter::None:
            return VK_FILTER_MAX_ENUM;
        case Filter::Nearest:
            return VK_FILTER_NEAREST;
        case Filter::Linear:
            return VK_FILTER_LINEAR;
    }
    ERROR("Invalid filter: {}", (uint32_t)filter);

    return VK_FILTER_MAX_ENUM;
}

VkSamplerAddressMode GetVulkanAddressMode(AddressMode addressMode)
{
    switch (addressMode)
    {
        case AddressMode::None:
            return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
        case AddressMode::Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case AddressMode::MirroredRepeat:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
        case AddressMode::Border:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    }

    ERROR("Invalid address mode: {}", (uint32_t)addressMode);

    return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

