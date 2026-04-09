#pragma once
#include <stdint.h>

enum class ImageFormat
{
	None = 0,

	R8,
	RG8,
	RGB8,
	RGBA8,
	R16,
	RG16,
	RGB16,
	RGBA16,
	R32,
	RG32,
	RGB32,
	RGBA32,
	R64,
	RG64,
	RGB64,
	RGBA64,

	BGRA8,
	D32
};

enum class ImageUsage : uint8_t
{
	None = 0,
	Sampler,
	Presentation,
	ColorAttachment,
	InputAttachment,
	DepthAttachment
};

inline ImageUsage operator|(ImageUsage lhs, ImageUsage rhs)
{
	return ImageUsage((uint8_t)lhs | (uint8_t)rhs);
}
inline ImageUsage operator&(ImageUsage lhs, ImageUsage rhs)
{
	return ImageUsage((uint8_t)lhs & (uint8_t)rhs);
}

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

enum class PipelineStage
{
	None = 0,
	Top,
	Bottom,
	Transfer,
	ColorOutput,
	EarlyFragmentTest,
	LateFragmentTest
};