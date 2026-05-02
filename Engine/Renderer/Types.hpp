#pragma once
#include <cstdint>

#define DefineBitOperators(Enum, Type) inline Enum operator|(Enum lhs, Enum rhs) \
{ \
    return Enum((Type)lhs | (Type)rhs); \
} \
inline Enum operator&(Enum lhs, Enum rhs) \
{ \
    return Enum((Type)lhs & (Type)rhs); \
} \


enum class ImageFormat
{
    None = 0,

    R8,
    RG8,
    RGB8,
    RGBA8,

    R8U,
    RG8U,
    RGB8U,
    RGBA8U,

    R8UNORM,
    RG8UNORM,
    RGB8UNORM,
    RGBA8UNORM,

    R16,
    RG16,
    RGB16,
    RGBA16,

    R16U,
    RG16U,
    RGB16U,
    RGBA16U,

    R16UNORM,
    RG16UNORM,
    RGB16UNORM,
    RGBA16UNORM,

    R32,
    RG32,
    RGB32,
    RGBA32,

    R32U,
    RG32U,
    RGB32U,
    RGBA32U,

    R64,
    RG64,
    RGB64,
    RGBA64,

    R64U,
    RG64U,
    RGB64U,
    RGBA64U,

    D32,
    D24_S8,

    BGRA8,
    BGRA8UNORM
};

enum class ImageLayout
{
    None = 0,
    Color,
    DepthStencil,
    ShaderRead,
    TransferSource,
    TransferDestination,
    PresentSource,
    General
};

enum class ImageUsage : uint64_t
{
    None = 0,
    Color = 1 << 1,
    DepthStencil = 1 << 2,
    Sampler = 1 << 3,
    TransferSource = 1 << 4,
    TransferDestination = 1 << 5,
    Storage = 1 << 6,
    InputAttachment = 1 << 7,
};

DefineBitOperators(ImageUsage, uint64_t);

enum class BufferUsage : uint64_t
{
    None = 0,
    VertexBuffer = 1 << 1,
    IndexBuffer = 1 << 2,
    UniformBuffer = 1 << 3,
    TransferSource = 1 << 4,
    TransferDestination = 1 << 5,
    Storage = 1 << 6,
};

DefineBitOperators(BufferUsage, uint64_t);

enum class ImageAspect
{
    None = 0,
    Color,
    Depth
};

enum class LoadOperation
{
    None = 0,
    Load,
    Clear,
    DontCare
};

enum class StoreOperation
{
    None = 0,
    Store,
    DontCare
};

enum class DescriptorType
{
    None = 0,
    Uniform,
    CombinedSampler,
    InputAttachment,
    StorageBuffer,
    StorageImage
};

enum class PipelineStage : uint64_t
{
    None = 1 << 0,
    TopOfPipe = 1 << 1,
    DrawIndirect = 1 << 2,
    VertexInput = 1 << 3,
    VertexShader = 1 << 4,
    TessellationControlShader = 1 << 5,
    TessellationEvaluationShader = 1 << 6,
    GeometryShader = 1 << 7, 
    FragmentShader = 1 << 8,
    EarlyFragmentTests = 1 << 9,
    LateFragmentTests = 1 << 10,
    ColorAttachmentOutput = 1 << 11,
    ComputeShader = 1 << 12,
    Transfer = 1 << 13,
    BottomOfPipe = 1 << 14,
    Host = 1 << 15,
    AllGraphics = 1 << 16,
    AllCommands = 1 << 17,
};

DefineBitOperators(PipelineStage, uint64_t);

enum class ShaderStage
{
    None = 0,
    Vertex,
    Fragment,
    Geometry,
    Tessellation,
    Compute,
};

enum class SampleCount
{
    One,
    Two,
    Four,
    Eight,
    Sixteen,
    ThirtyTwo,
    SixtyFour
};

enum class MemoryProperty : uint64_t
{
    None = 0,
    DeviceLocal = 1 << 1,
    HostVisible = 1 << 2,
    HostCoherent = 1 << 3
};

DefineBitOperators(MemoryProperty, uint64_t)

enum class DeviceType
{
    None = 0,
    Dedicated,
    Integrated,
    VirtualGpu,
    Cpu,
};

enum class Filter
{
    None = 0,
    Nearest,
    Linear,
};

enum class AddressMode
{
    None = 0,
    Repeat,
    MirroredRepeat,
    Border,
};

enum class PresentMode
{
    None = 0,
    Fifo,
    Mailbox,
    Immediate,
};

enum class CullMode
{
    None = 0,
    Front,
    Back,
};

enum class FrontFace
{
    None = 0,
    Clockwise, 
    CounterClockwise,
};

enum class PrimitiveType
{
    None = 0,
    Triangle, 
    Line, 
    Point,
};

enum class InputRate
{
    None = 0,
    Vertex,
    Instance
};

enum class PipelineBindPoint
{
    None = 0,
    Graphic,
    Compute,
    RayTracing
};