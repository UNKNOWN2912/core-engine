#pragma once
#include <cstdint>
#include <string>

struct ShaderRef
{
	uint32_t key = 0;
};

struct TextureRef
{
	uint32_t key = 0;
};

struct Material
{
	std::string shader;

	std::string albedo;
	std::string normal;
	std::string specular;
};