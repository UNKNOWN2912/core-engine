#pragma once
#include <cstdint>
#include <glm/glm.hpp>

#define DefineBitOperators(Enum, Type)          \
    inline Enum operator|(Enum lhs, Enum rhs)   \
    {                                           \
        return Enum((Type)lhs | (Type)rhs);     \
    }                                           \
    inline Enum operator&(Enum lhs, Enum rhs)   \
    {                                           \
        return Enum((Type)lhs & (Type)rhs);     \
    }                                           \
    inline void operator|=(Enum &lhs, Enum rhs) \
    {                                           \
        lhs = lhs | rhs;                        \
    }

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
    D24S8,

    BGRA8,
    BGRA8UNORM
};

enum class ColorSpace
{
    None,
    SRGBNonLinear,
    DisplayP3NonLinear,
    ExtendedSRGBLinear,
    DisplayP3Linear,
    DciP3Nonlinear,
    Bt709Linear,
    Bt709NonLinear,
    Bt2020Linear,
    Hdr10St2084,
    Dolbyvision,
    Hdr10Hlg,
    AdobeRGBLinear,
    AdobeRGBNonlinear,
    PassThrough,
    ExtendedSRGBNonlinear,
    DisplayNative,
};

enum class ImageLayout
{
    None = 0,
    ColorAttachment,
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
    ColorAttachment = 1 << 1,
    DepthStencil = 1 << 2,
    Sampler = 1 << 3,
    TransferSource = 1 << 4,
    TransferDestination = 1 << 5,
    Storage = 1 << 6,
    InputAttachment = 1 << 7,
};

inline ImageUsage operator|(ImageUsage lhs, ImageUsage rhs)
{
    return ImageUsage((uint64_t)lhs | (uint64_t)rhs);
}
inline ImageUsage operator&(ImageUsage lhs, ImageUsage rhs)
{
    return ImageUsage((uint64_t)lhs & (uint64_t)rhs);
}
inline void operator|=(ImageUsage &lhs, ImageUsage rhs)
{
    lhs = lhs | rhs;
};

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
    All,
};

enum class SampleCount
{
    None = 0,
    One,
    Two,
    Four,
    Eight,
    Sixteen,
    ThirtyTwo,
    SixtyFour,
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
    FifoLatestReady,
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

enum class ViewType
{
    None = 0,
    OneDimensional,
    TwoDimensional,
    ThreeDimensional,
    Cube,
    OneDimensionalArray,
    TwoDimensionalArray,
    CubeArray
};

enum class ImageType
{
    None,
    OneDimensional,
    TwoDimensional,
    ThreeDimensional,
};
