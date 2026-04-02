#pragma once

enum class ImageFormat
{
	None,
	BGRA8,
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
};

enum class TextureFilter
{
	Nearest, Linear
};

enum class TextureWrap
{
	Repeat,
	MirrorRepeat
};

