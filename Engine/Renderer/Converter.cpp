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
    case ImageFormat::D24S8:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case ImageFormat::BGRA8:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case ImageFormat::R8U:
        return VK_FORMAT_R8_UINT;
    case ImageFormat::RG8U:
        return VK_FORMAT_R8G8_UINT;
    case ImageFormat::RGB8U:
        return VK_FORMAT_R8G8B8_UINT;
    case ImageFormat::RGBA8U:
        return VK_FORMAT_R8G8B8A8_UINT;
    case ImageFormat::R8UNORM:
        return VK_FORMAT_R8_UNORM;
    case ImageFormat::RG8UNORM:
        return VK_FORMAT_R8G8_UNORM;
    case ImageFormat::RGB8UNORM:
        return VK_FORMAT_R8G8B8_UNORM;
    case ImageFormat::RGBA8UNORM:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case ImageFormat::R16UNORM:
        return VK_FORMAT_R16_UNORM;
    case ImageFormat::RG16UNORM:
        return VK_FORMAT_R16G16_UNORM;
    case ImageFormat::RGB16UNORM:
        return VK_FORMAT_R16G16B16_UNORM;
    case ImageFormat::RGBA16UNORM:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case ImageFormat::BGRA8UNORM:
        return VK_FORMAT_B8G8R8A8_UNORM;
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
    case ImageLayout::ColorAttachment:
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
    case ImageLayout::General:
        return VK_IMAGE_LAYOUT_GENERAL;
    }
    ERROR("Invalid image layout: {}", (uint32_t)imageLayout);

    return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkImageUsageFlags GetVulkanImageUsage(ImageUsage imageUsage)
{
    VkImageUsageFlags usage = 0;

    if ((ImageUsage::None & imageUsage) == ImageUsage::None)
    {
        usage |= 0;
    }
    if ((ImageUsage::ColorAttachment & imageUsage) == ImageUsage::ColorAttachment)
    {
        usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
    if ((ImageUsage::DepthStencil & imageUsage) == ImageUsage::DepthStencil)
    {
        usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if ((ImageUsage::Sampler & imageUsage) == ImageUsage::Sampler)
    {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if ((ImageUsage::TransferSource & imageUsage) == ImageUsage::TransferSource)
    {
        usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }
    if ((ImageUsage::TransferDestination & imageUsage) == ImageUsage::TransferDestination)
    {
        usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
    if ((ImageUsage::Storage & imageUsage) == ImageUsage::Storage)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    if ((ImageUsage::InputAttachment & imageUsage) == ImageUsage::InputAttachment)
    {
        usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    }

    return usage;
}

VkBufferUsageFlags GetVulkanBufferUsage(BufferUsage bufferUsage)
{
    VkBufferUsageFlags usage = 0;

    if ((BufferUsage::VertexBuffer & bufferUsage) == BufferUsage::VertexBuffer)
    {
        usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    }
    if ((BufferUsage::IndexBuffer & bufferUsage) == BufferUsage::IndexBuffer)
    {
        usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }
    if ((BufferUsage::UniformBuffer & bufferUsage) == BufferUsage::UniformBuffer)
    {
        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    }
    if ((BufferUsage::TransferSource & bufferUsage) == BufferUsage::TransferSource)
    {
        usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    }
    if ((BufferUsage::TransferDestination & bufferUsage) == BufferUsage::TransferDestination)
    {
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    if ((BufferUsage::Storage & bufferUsage) == BufferUsage::Storage)
    {
        usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

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
    case DescriptorType::StorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DescriptorType::StorageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    }
    ERROR("Invalid descriptor type: {}", (uint32_t)descriptorType);

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkPipelineStageFlags GetVulkanPipelineStage(PipelineStage pipelineStage)
{
    VkPipelineStageFlags stage = 0;

    if ((PipelineStage::None & pipelineStage) == PipelineStage::None)
    {
        stage |= VK_PIPELINE_STAGE_NONE;
    }
    if ((PipelineStage::TopOfPipe & pipelineStage) == PipelineStage::TopOfPipe)
    {
        stage |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    }
    if ((PipelineStage::DrawIndirect & pipelineStage) == PipelineStage::DrawIndirect)
    {
        stage |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
    }
    if ((PipelineStage::VertexInput & pipelineStage) == PipelineStage::VertexInput)
    {
        stage |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    }
    if ((PipelineStage::VertexShader & pipelineStage) == PipelineStage::VertexShader)
    {
        stage |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    }
    if ((PipelineStage::TessellationControlShader & pipelineStage) == PipelineStage::TessellationControlShader)
    {
        stage |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
    }
    if ((PipelineStage::TessellationEvaluationShader & pipelineStage) == PipelineStage::TessellationEvaluationShader)
    {
        stage |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
    }
    if ((PipelineStage::GeometryShader & pipelineStage) == PipelineStage::GeometryShader)
    {
        stage |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
    }
    if ((PipelineStage::FragmentShader & pipelineStage) == PipelineStage::FragmentShader)
    {
        stage |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    if ((PipelineStage::EarlyFragmentTests & pipelineStage) == PipelineStage::EarlyFragmentTests)
    {
        stage |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    }
    if ((PipelineStage::LateFragmentTests & pipelineStage) == PipelineStage::LateFragmentTests)
    {
        stage |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    }
    if ((PipelineStage::ColorAttachmentOutput & pipelineStage) == PipelineStage::ColorAttachmentOutput)
    {
        stage |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    }
    if ((PipelineStage::ComputeShader & pipelineStage) == PipelineStage::ComputeShader)
    {
        stage |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    }
    if ((PipelineStage::Transfer & pipelineStage) == PipelineStage::Transfer)
    {
        stage |= VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    if ((PipelineStage::BottomOfPipe & pipelineStage) == PipelineStage::BottomOfPipe)
    {
        stage |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    if ((PipelineStage::Host & pipelineStage) == PipelineStage::Host)
    {
        stage |= VK_PIPELINE_STAGE_HOST_BIT;
    }
    if ((PipelineStage::AllGraphics & pipelineStage) == PipelineStage::AllGraphics)
    {
        stage |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    }
    if ((PipelineStage::AllCommands & pipelineStage) == PipelineStage::AllCommands)
    {
        stage |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
    }

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
    case ShaderStage::All:
        return VK_SHADER_STAGE_ALL;
    }
    ERROR("Invalid shader stage: {}", (uint32_t)shaderStage);

    return 0;
}

VkSampleCountFlagBits GetVulkanSampleCount(SampleCount sampleCount)
{
    switch (sampleCount)
    {
    case SampleCount::None:
        return (VkSampleCountFlagBits)0;
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
    VkMemoryPropertyFlags property = 0;

    if ((MemoryProperty::DeviceLocal & memoryProperty) == MemoryProperty::DeviceLocal)
    {
        property |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    }
    if ((MemoryProperty::HostVisible & memoryProperty) == MemoryProperty::HostVisible)
    {
        property |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    }
    if ((MemoryProperty::HostCoherent & memoryProperty) == MemoryProperty::HostCoherent)
    {
        property |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    }

    return property;
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

VkPresentModeKHR GetVulkanPresentMode(PresentMode presentMode)
{
    switch (presentMode)
    {
    case PresentMode::None:
        return VK_PRESENT_MODE_MAX_ENUM_KHR;
    case PresentMode::Fifo:
        return VK_PRESENT_MODE_FIFO_KHR;
    case PresentMode::Mailbox:
        return VK_PRESENT_MODE_MAILBOX_KHR;
    case PresentMode::Immediate:
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    case PresentMode::FifoLatestReady:
        return VK_PRESENT_MODE_FIFO_LATEST_READY_KHR;
        break;
    }

    ERROR("Invalid present mode");

    return VK_PRESENT_MODE_MAX_ENUM_KHR;
}

VkCullModeFlags GetVulkanCullMode(CullMode mode)
{
    CHROME_TRACE_FUNCTION();
    switch (mode)
    {
    case CullMode::None:
        return VK_CULL_MODE_NONE;
        break;
    case CullMode::Front:
        return VK_CULL_MODE_FRONT_BIT;
        break;
    case CullMode::Back:
        return VK_CULL_MODE_BACK_BIT;
        break;
    }
    ERROR("Invalid cull mode");
    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
}

VkVertexInputRate GetVulkanInputRate(InputRate inputRate)
{
    switch (inputRate)
    {
    case InputRate::None:
        return VK_VERTEX_INPUT_RATE_MAX_ENUM;
    case InputRate::Vertex:
        return VK_VERTEX_INPUT_RATE_VERTEX;
    case InputRate::Instance:
        return VK_VERTEX_INPUT_RATE_INSTANCE;
    }
    ERROR("Invalid input rate");

    return VK_VERTEX_INPUT_RATE_MAX_ENUM;
}

VkPipelineBindPoint GetVulkanPipelineBindPoint(PipelineBindPoint bindPoint)
{
    switch (bindPoint)
    {
    case PipelineBindPoint::None:
        return VK_PIPELINE_BIND_POINT_MAX_ENUM;
    case PipelineBindPoint::Graphic:
        return VK_PIPELINE_BIND_POINT_GRAPHICS;
    case PipelineBindPoint::Compute:
        return VK_PIPELINE_BIND_POINT_COMPUTE;
    case PipelineBindPoint::RayTracing:
        return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
    }

    ERROR("Invalid pipeline bind point");
    return VK_PIPELINE_BIND_POINT_MAX_ENUM;
}

VkColorSpaceKHR GetVulkanColorSpace(ColorSpace colorspace)
{
    CHROME_TRACE_FUNCTION();

    switch (colorspace)
    {
    case ColorSpace::SRGBNonLinear:
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    case ColorSpace::DisplayP3NonLinear:
        return VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT;
    case ColorSpace::ExtendedSRGBLinear:
        return VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;
    case ColorSpace::DisplayP3Linear:
        return VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT;
    case ColorSpace::DciP3Nonlinear:
        return VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT;
    case ColorSpace::Bt709Linear:
        return VK_COLOR_SPACE_BT709_LINEAR_EXT;
    case ColorSpace::Bt709NonLinear:
        return VK_COLOR_SPACE_BT709_NONLINEAR_EXT;
    case ColorSpace::Bt2020Linear:
        return VK_COLOR_SPACE_BT2020_LINEAR_EXT;
    case ColorSpace::Hdr10St2084:
        return VK_COLOR_SPACE_HDR10_ST2084_EXT;
    case ColorSpace::Dolbyvision:
        return VK_COLOR_SPACE_DOLBYVISION_EXT;
    case ColorSpace::Hdr10Hlg:
        return VK_COLOR_SPACE_HDR10_HLG_EXT;
    case ColorSpace::AdobeRGBLinear:
        return VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT;
    case ColorSpace::AdobeRGBNonlinear:
        return VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT;
    case ColorSpace::PassThrough:
        return VK_COLOR_SPACE_PASS_THROUGH_EXT;
    case ColorSpace::ExtendedSRGBNonlinear:
        return VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT;
    case ColorSpace::DisplayNative:
        return VK_COLOR_SPACE_DISPLAY_NATIVE_AMD;
    case ColorSpace::None:
        return (VkColorSpaceKHR)0;
        break;
    }

    ERROR("Invalid colorspace");
    return VK_COLOR_SPACE_MAX_ENUM_KHR;
}

VkImageViewType GetVulkanViewType(ViewType viewtype)
{
    switch (viewtype)
    {
    case ViewType::None:
        return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    case ViewType::OneDimensional:
        return VK_IMAGE_VIEW_TYPE_1D;
    case ViewType::TwoDimensional:
        return VK_IMAGE_VIEW_TYPE_2D;
    case ViewType::ThreeDimensional:
        return VK_IMAGE_VIEW_TYPE_3D;
    case ViewType::Cube:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case ViewType::OneDimensionalArray:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case ViewType::TwoDimensionalArray:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case ViewType::CubeArray:
        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
        break;
    }

    ERROR("Invalid view type");
    return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

VkImageType GetVulkanImageType(ImageType type)
{
    switch (type)
    {
    case ImageType::None:
        return VK_IMAGE_TYPE_MAX_ENUM;
    case ImageType::OneDimensional:
        return VK_IMAGE_TYPE_1D;
    case ImageType::TwoDimensional:
        return VK_IMAGE_TYPE_2D;
    case ImageType::ThreeDimensional:
        return VK_IMAGE_TYPE_3D;
    }

    ERROR("Invalid image type");
    return VK_IMAGE_TYPE_MAX_ENUM;
}

VkPrimitiveTopology GetVulkanPrimitive(PrimitiveType primitive)
{
    CHROME_TRACE_FUNCTION();
    switch (primitive)
    {
    case PrimitiveType::None:
        return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        break;
    case PrimitiveType::Triangle:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        break;
    case PrimitiveType::Line:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        break;
    case PrimitiveType::Point:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        break;
    };

    ERROR("Invalid primitive");
    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkFrontFace GetVulkanFrontsFace(FrontFace face)
{
    CHROME_TRACE_FUNCTION();
    switch (face)
    {
    case FrontFace::None:
        return VK_FRONT_FACE_MAX_ENUM;
        break;
    case FrontFace::Clockwise:
        return VK_FRONT_FACE_CLOCKWISE;
        break;
    case FrontFace::CounterClockwise:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        break;
    }

    ERROR("Invalid front face");
    return VK_FRONT_FACE_MAX_ENUM;
}

ImageFormat GetNativeImageFormat(VkFormat imageFormat)
{
    switch (imageFormat)
    {
    case VK_FORMAT_UNDEFINED:
        return ImageFormat::None;
    case VK_FORMAT_R8_SRGB:
        return ImageFormat::R8;
    case VK_FORMAT_R8G8_SRGB:
        return ImageFormat::RG8;
    case VK_FORMAT_R8G8B8_SRGB:
        return ImageFormat::RGB8;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return ImageFormat::RGBA8;
    case VK_FORMAT_R16_SFLOAT:
        return ImageFormat::R16;
    case VK_FORMAT_R16G16_SFLOAT:
        return ImageFormat::RG16;
    case VK_FORMAT_R16G16B16_SFLOAT:
        return ImageFormat::RGB16;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return ImageFormat::RGBA16;
    case VK_FORMAT_R16_UINT:
        return ImageFormat::R16U;
    case VK_FORMAT_R16G16_UINT:
        return ImageFormat::RG16U;
    case VK_FORMAT_R16G16B16_UINT:
        return ImageFormat::RGB16U;
    case VK_FORMAT_R16G16B16A16_UINT:
        return ImageFormat::RGBA16U;
    case VK_FORMAT_R32_SFLOAT:
        return ImageFormat::R32;
    case VK_FORMAT_R32G32_SFLOAT:
        return ImageFormat::RG32;
    case VK_FORMAT_R32G32B32_SFLOAT:
        return ImageFormat::RGB32;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return ImageFormat::RGBA32;
    case VK_FORMAT_R32_UINT:
        return ImageFormat::R32U;
    case VK_FORMAT_R32G32_UINT:
        return ImageFormat::RG32U;
    case VK_FORMAT_R32G32B32_UINT:
        return ImageFormat::RGB32U;
    case VK_FORMAT_R32G32B32A32_UINT:
        return ImageFormat::RGBA32U;
    case VK_FORMAT_R64_SFLOAT:
        return ImageFormat::R64;
    case VK_FORMAT_R64G64_SFLOAT:
        return ImageFormat::RG64;
    case VK_FORMAT_R64G64B64_SFLOAT:
        return ImageFormat::RGB64;
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return ImageFormat::RGBA64;
    case VK_FORMAT_R64_UINT:
        return ImageFormat::R64U;
    case VK_FORMAT_R64G64_UINT:
        return ImageFormat::RG64U;
    case VK_FORMAT_R64G64B64_UINT:
        return ImageFormat::RGB64U;
    case VK_FORMAT_R64G64B64A64_UINT:
        return ImageFormat::RGBA64U;
    case VK_FORMAT_D32_SFLOAT:
        return ImageFormat::D32;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return ImageFormat::D24S8;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return ImageFormat::BGRA8;
    case VK_FORMAT_R8_UINT:
        return ImageFormat::R8U;
    case VK_FORMAT_R8G8_UINT:
        return ImageFormat::RG8U;
    case VK_FORMAT_R8G8B8_UINT:
        return ImageFormat::RGB8U;
    case VK_FORMAT_R8G8B8A8_UINT:
        return ImageFormat::RGBA8U;
    case VK_FORMAT_R8_UNORM:
        return ImageFormat::R8UNORM;
    case VK_FORMAT_R8G8_UNORM:
        return ImageFormat::RG8UNORM;
    case VK_FORMAT_R8G8B8_UNORM:
        return ImageFormat::RGB8UNORM;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return ImageFormat::RGBA8UNORM;
    case VK_FORMAT_R16_UNORM:
        return ImageFormat::R16UNORM;
    case VK_FORMAT_R16G16_UNORM:
        return ImageFormat::RG16UNORM;
    case VK_FORMAT_R16G16B16_UNORM:
        return ImageFormat::RGB16UNORM;
    case VK_FORMAT_R16G16B16A16_UNORM:
        return ImageFormat::RGBA16UNORM;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return ImageFormat::BGRA8UNORM;
    }

    ERROR("Invalid format: {}", (uint32_t)imageFormat);

    return ImageFormat::None;
}
ImageLayout GetNativeImageLayout(VkImageLayout imageLayout)
{
    switch (imageLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        return ImageLayout::None;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        return ImageLayout::ColorAttachment;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        return ImageLayout::DepthStencil;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        return ImageLayout::ShaderRead;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        return ImageLayout::TransferSource;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        return ImageLayout::TransferDestination;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        return ImageLayout::PresentSource;
    case VK_IMAGE_LAYOUT_GENERAL:
        return ImageLayout::General;
    }
    ERROR("Invalid image layout: {}", (uint32_t)imageLayout);

    return ImageLayout::None;
}
ImageUsage GetNativeImageUsage(VkImageUsageFlags imageUsage)
{
    ImageUsage usage = ImageUsage::None;
    if ((imageUsage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        usage |= ImageUsage::ColorAttachment;
    }
    if ((imageUsage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        usage |= ImageUsage::DepthStencil;
    }
    if ((imageUsage & VK_IMAGE_USAGE_SAMPLED_BIT) == VK_IMAGE_USAGE_SAMPLED_BIT)
    {
        usage |= ImageUsage::Sampler;
    }
    if ((imageUsage & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) == VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        usage |= ImageUsage::TransferSource;
    }
    if ((imageUsage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) == VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        usage |= ImageUsage::TransferDestination;
    }
    if ((imageUsage & VK_IMAGE_USAGE_STORAGE_BIT) == VK_IMAGE_USAGE_STORAGE_BIT)
    {
        usage |= ImageUsage::Storage;
    }
    if ((imageUsage & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT) == VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT)
    {
        usage |= ImageUsage::InputAttachment;
    }
    return usage;
}
BufferUsage GetNativeBufferUsage(VkBufferUsageFlags bufferUsage)
{
    BufferUsage usage = BufferUsage::None;

    if ((VK_BUFFER_USAGE_VERTEX_BUFFER_BIT & bufferUsage) == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)
    {
        usage |= BufferUsage::VertexBuffer;
    }
    if ((VK_BUFFER_USAGE_INDEX_BUFFER_BIT & bufferUsage) == VK_BUFFER_USAGE_INDEX_BUFFER_BIT)
    {
        usage |= BufferUsage::IndexBuffer;
    }
    if ((VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT & bufferUsage) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
    {
        usage |= BufferUsage::UniformBuffer;
    }
    if ((VK_BUFFER_USAGE_TRANSFER_SRC_BIT & bufferUsage) == VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
    {
        usage |= BufferUsage::TransferSource;
    }
    if ((VK_BUFFER_USAGE_TRANSFER_DST_BIT & bufferUsage) == VK_BUFFER_USAGE_TRANSFER_DST_BIT)
    {
        usage |= BufferUsage::TransferDestination;
    }
    if ((VK_BUFFER_USAGE_STORAGE_BUFFER_BIT & bufferUsage) == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
    {
        usage |= BufferUsage::Storage;
    }

    return usage;
}
ImageAspect GetNativeImageAspect(VkImageAspectFlags imageAspect)
{
    switch (imageAspect)
    {
    case VK_IMAGE_ASPECT_NONE:
        return ImageAspect::None;
    case VK_IMAGE_ASPECT_COLOR_BIT:
        return ImageAspect::Color;
    case VK_IMAGE_ASPECT_DEPTH_BIT:
        return ImageAspect::Depth;
    };

    ERROR("Invalid image aspect: {}", (uint32_t)imageAspect);

    return ImageAspect::None;
}
LoadOperation GetNativeLoadOperation(VkAttachmentLoadOp loadOperation)
{
    switch (loadOperation)
    {
    case VK_ATTACHMENT_LOAD_OP_NONE:
        return LoadOperation::None;
    case VK_ATTACHMENT_LOAD_OP_LOAD:
        return LoadOperation::Load;
    case VK_ATTACHMENT_LOAD_OP_CLEAR:
        return LoadOperation::Clear;
    case VK_ATTACHMENT_LOAD_OP_DONT_CARE:
        return LoadOperation::DontCare;
    }
    ERROR("Invalid load operation: {}", (uint32_t)loadOperation);

    return LoadOperation::None;
}
StoreOperation GetNativeStoreOperation(VkAttachmentStoreOp storeOperation)
{
    switch (storeOperation)
    {
    case VK_ATTACHMENT_STORE_OP_NONE:
        return StoreOperation::None;
    case VK_ATTACHMENT_STORE_OP_STORE:
        return StoreOperation::Store;
    case VK_ATTACHMENT_STORE_OP_DONT_CARE:
        return StoreOperation::DontCare;
    }
    ERROR("Invalid store operation: {}", (uint32_t)storeOperation);

    return StoreOperation::None;
}
DescriptorType GetNativeDescriptorType(VkDescriptorType descriptorType)
{
    switch (descriptorType)
    {
    case VK_DESCRIPTOR_TYPE_MAX_ENUM:
        return DescriptorType::None;
    case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
        return DescriptorType::Uniform;
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        return DescriptorType::CombinedSampler;
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        return DescriptorType::InputAttachment;
    case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
        return DescriptorType::StorageBuffer;
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        return DescriptorType::StorageImage;
    }
    ERROR("Invalid descriptor type: {}", (uint32_t)descriptorType);

    return DescriptorType::None;
}
PipelineStage GetNativePipelineStage(VkPipelineStageFlags pipelineStage)
{
    PipelineStage stage = PipelineStage::None;

    if ((VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT & pipelineStage) == VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
    {
        stage |= PipelineStage::TopOfPipe;
    }
    if ((VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT & pipelineStage) == VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT)
    {
        stage |= PipelineStage::DrawIndirect;
    }
    if ((VK_PIPELINE_STAGE_VERTEX_INPUT_BIT & pipelineStage) == VK_PIPELINE_STAGE_VERTEX_INPUT_BIT)
    {
        stage |= PipelineStage::VertexInput;
    }
    if ((VK_PIPELINE_STAGE_VERTEX_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_VERTEX_SHADER_BIT)
    {
        stage |= PipelineStage::VertexShader;
    }
    if ((VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT)
    {
        stage |= PipelineStage::TessellationControlShader;
    }
    if ((VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT)
    {
        stage |= PipelineStage::TessellationEvaluationShader;
    }
    if ((VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT)
    {
        stage |= PipelineStage::GeometryShader;
    }
    if ((VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT)
    {
        stage |= PipelineStage::FragmentShader;
    }
    if ((VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT & pipelineStage) == VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT)
    {
        stage |= PipelineStage::EarlyFragmentTests;
    }
    if ((VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT & pipelineStage) == VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT)
    {
        stage |= PipelineStage::LateFragmentTests;
    }
    if ((VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT & pipelineStage) == VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
    {
        stage |= PipelineStage::ColorAttachmentOutput;
    }
    if ((VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT & pipelineStage) == VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT)
    {
        stage |= PipelineStage::ComputeShader;
    }
    if ((VK_PIPELINE_STAGE_TRANSFER_BIT & pipelineStage) == VK_PIPELINE_STAGE_TRANSFER_BIT)
    {
        stage |= PipelineStage::Transfer;
    }
    if ((VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT & pipelineStage) == VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT)
    {
        stage |= PipelineStage::BottomOfPipe;
    }
    if ((VK_PIPELINE_STAGE_HOST_BIT & pipelineStage) == VK_PIPELINE_STAGE_HOST_BIT)
    {
        stage |= PipelineStage::Host;
    }
    if ((VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT & pipelineStage) == VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT)
    {
        stage |= PipelineStage::AllGraphics;
    }
    if ((VK_PIPELINE_STAGE_ALL_COMMANDS_BIT & pipelineStage) == VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
    {
        stage |= PipelineStage::AllCommands;
    }

    return stage;
}
ShaderStage GetNativeShaderStage(VkShaderStageFlags shaderStage)
{
    switch (shaderStage)
    {
    case VK_SHADER_STAGE_VERTEX_BIT:
        return ShaderStage::Vertex;
    case VK_SHADER_STAGE_FRAGMENT_BIT:
        return ShaderStage::Fragment;
    case VK_SHADER_STAGE_GEOMETRY_BIT:
        return ShaderStage::Geometry;
    case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
        return ShaderStage::Tessellation;
    case VK_SHADER_STAGE_COMPUTE_BIT:
        return ShaderStage::Compute;
    case VK_SHADER_STAGE_ALL:
        return ShaderStage::All;
    }
    ERROR("Invalid shader stage: {}", (uint32_t)shaderStage);

    return ShaderStage::None;
}
SampleCount GetNativeSampleCount(VkSampleCountFlagBits sampleCount)
{
    switch (sampleCount)
    {
    case VK_SAMPLE_COUNT_1_BIT:
        return SampleCount::One;
    case VK_SAMPLE_COUNT_2_BIT:
        return SampleCount::Two;
    case VK_SAMPLE_COUNT_4_BIT:
        return SampleCount::Four;
    case VK_SAMPLE_COUNT_8_BIT:
        return SampleCount::Eight;
    case VK_SAMPLE_COUNT_16_BIT:
        return SampleCount::Sixteen;
    case VK_SAMPLE_COUNT_32_BIT:
        return SampleCount::ThirtyTwo;
    case VK_SAMPLE_COUNT_64_BIT:
        return SampleCount::SixtyFour;
    }
    ERROR("Invalid sample count: {}", (uint32_t)sampleCount);

    return SampleCount::None;
}
MemoryProperty GetNativeMemoryProperty(VkMemoryPropertyFlags memoryProperty)
{
    MemoryProperty property = MemoryProperty::None;

    if ((VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT & memoryProperty) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
    {
        property |= MemoryProperty::DeviceLocal;
    }
    if ((VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT & memoryProperty) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    {
        property |= MemoryProperty::HostVisible;
    }
    if ((VK_MEMORY_PROPERTY_HOST_COHERENT_BIT & memoryProperty) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
    {
        property |= MemoryProperty::HostCoherent;
    }

    return property;
}
DeviceType GetNativeDeviceType(VkPhysicalDeviceType deviceType)
{
    switch (deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        return DeviceType::None;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        return DeviceType::Dedicated;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        return DeviceType::Integrated;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        return DeviceType::VirtualGpu;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        return DeviceType::Cpu;
    }
    ERROR("Invalid device type: {}", (uint32_t)deviceType);

    return DeviceType::None;
}
Filter GetNativeFilter(VkFilter filter)
{
    switch (filter)
    {
    case VK_FILTER_MAX_ENUM:
        return Filter::None;
    case VK_FILTER_NEAREST:
        return Filter::Nearest;
    case VK_FILTER_LINEAR:
        return Filter::Linear;
    }
    ERROR("Invalid filter: {}", (uint32_t)filter);

    return Filter::None;
}
AddressMode GetNativeAddressMode(VkSamplerAddressMode addressMode)
{
    switch (addressMode)
    {
    case VK_SAMPLER_ADDRESS_MODE_MAX_ENUM:
        return AddressMode::None;
    case VK_SAMPLER_ADDRESS_MODE_REPEAT:
        return AddressMode::Repeat;
    case VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT:
        return AddressMode::MirroredRepeat;
    case VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER:
        return AddressMode::Border;
    }

    ERROR("Invalid address mode: {}", (uint32_t)addressMode);

    return AddressMode::None;
}
PresentMode GetNativePresentMode(VkPresentModeKHR presentMode)
{
    switch (presentMode)
    {
    case VK_PRESENT_MODE_MAX_ENUM_KHR:
        return PresentMode::None;
    case VK_PRESENT_MODE_FIFO_KHR:
        return PresentMode::Fifo;
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return PresentMode::Mailbox;
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return PresentMode::Immediate;
    case VK_PRESENT_MODE_FIFO_LATEST_READY_KHR:
        return PresentMode::FifoLatestReady;
    }

    ERROR("Invalid present mode");

    return PresentMode::None;
}
FrontFace GetNativeFrontsFace(VkFrontFace face)
{
    CHROME_TRACE_FUNCTION();
    switch (face)
    {
    case VK_FRONT_FACE_MAX_ENUM:
        return FrontFace::None;
        break;
    case VK_FRONT_FACE_CLOCKWISE:
        return FrontFace::Clockwise;
        break;
    case VK_FRONT_FACE_COUNTER_CLOCKWISE:
        return FrontFace::CounterClockwise;
        break;
    }

    ERROR("Invalid front face");
    return FrontFace::None;
}
PrimitiveType GetNativePrimitive(VkPrimitiveTopology primitive)
{
    CHROME_TRACE_FUNCTION();

    switch (primitive)
    {
    case VK_PRIMITIVE_TOPOLOGY_MAX_ENUM:
        return PrimitiveType::None;
        break;
    case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
        return PrimitiveType::Triangle;
        break;
    case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
        return PrimitiveType::Line;
        break;
    case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
        return PrimitiveType::Point;
        break;
    };

    ERROR("Invalid primitive");
    return PrimitiveType::None;
}
CullMode GetNativeCullMode(VkCullModeFlags mode)
{
    CHROME_TRACE_FUNCTION();
    switch (mode)
    {
    case VK_CULL_MODE_NONE:
        return CullMode::None;
        break;
    case VK_CULL_MODE_FRONT_BIT:
        return CullMode::Front;
        break;
    case VK_CULL_MODE_BACK_BIT:
        return CullMode::Back;
        break;
    }
    ERROR("Invalid cull mode");
    return CullMode::None;
}
InputRate GetNativeInputRate(VkVertexInputRate inputRate)
{
    switch (inputRate)
    {
    case VK_VERTEX_INPUT_RATE_MAX_ENUM:
        return InputRate::None;
    case VK_VERTEX_INPUT_RATE_VERTEX:
        return InputRate::Vertex;
    case VK_VERTEX_INPUT_RATE_INSTANCE:
        return InputRate::Instance;
    }
    ERROR("Invalid input rate");

    return InputRate::None;
}
PipelineBindPoint GetNativePipelineBindPoint(VkPipelineBindPoint bindPoint)
{
    switch (bindPoint)
    {
    case VK_PIPELINE_BIND_POINT_MAX_ENUM:
        return PipelineBindPoint::None;
    case VK_PIPELINE_BIND_POINT_GRAPHICS:
        return PipelineBindPoint::Graphic;
    case VK_PIPELINE_BIND_POINT_COMPUTE:
        return PipelineBindPoint::Compute;
    case VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR:
        return PipelineBindPoint::RayTracing;
    }

    ERROR("Invalid pipeline bind point");
    return PipelineBindPoint::None;
}
ColorSpace GetNativeColorSpace(VkColorSpaceKHR colorspace)
{

    switch (colorspace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return ColorSpace::SRGBNonLinear;
    case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
        return ColorSpace::DisplayP3NonLinear;
    case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        return ColorSpace::ExtendedSRGBLinear;
    case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
        return ColorSpace::DisplayP3Linear;
    case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
        return ColorSpace::DciP3Nonlinear;
    case VK_COLOR_SPACE_BT709_LINEAR_EXT:
        return ColorSpace::Bt709Linear;
    case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
        return ColorSpace::Bt709NonLinear;
    case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
        return ColorSpace::Bt2020Linear;
    case VK_COLOR_SPACE_HDR10_ST2084_EXT:
        return ColorSpace::Hdr10St2084;
    case VK_COLOR_SPACE_DOLBYVISION_EXT:
        return ColorSpace::Dolbyvision;
    case VK_COLOR_SPACE_HDR10_HLG_EXT:
        return ColorSpace::Hdr10Hlg;
    case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
        return ColorSpace::AdobeRGBLinear;
    case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
        return ColorSpace::AdobeRGBNonlinear;
    case VK_COLOR_SPACE_PASS_THROUGH_EXT:
        return ColorSpace::PassThrough;
    case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
        return ColorSpace::ExtendedSRGBNonlinear;
    case VK_COLOR_SPACE_DISPLAY_NATIVE_AMD:
        return ColorSpace::DisplayNative;
    }

    ERROR("Invalid colorspace");
    return ColorSpace::None;
}
ViewType GetNativeViewType(VkImageViewType viewtype)
{
    switch (viewtype)
    {
    case VK_IMAGE_VIEW_TYPE_1D:
        return ViewType::OneDimensional;
    case VK_IMAGE_VIEW_TYPE_2D:
        return ViewType::TwoDimensional;
    case VK_IMAGE_VIEW_TYPE_3D:
        return ViewType::ThreeDimensional;
    case VK_IMAGE_VIEW_TYPE_CUBE:
        return ViewType::Cube;
    case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
        return ViewType::OneDimensionalArray;
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        return ViewType::TwoDimensionalArray;
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
        return ViewType::CubeArray;
    case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
        return ViewType::None;
    }

    ERROR("Invalid view type");
    return ViewType::None;
}
ImageType GetNativeImageType(VkImageType type)
{
    switch (type)
    {
    case VK_IMAGE_TYPE_1D:
        return ImageType::OneDimensional;
    case VK_IMAGE_TYPE_2D:
        return ImageType::TwoDimensional;
    case VK_IMAGE_TYPE_3D:
        return ImageType::ThreeDimensional;
    case VK_IMAGE_TYPE_MAX_ENUM:
        return ImageType::None;
    }

    ERROR("Invalid image type");
    return ImageType::None;
}
