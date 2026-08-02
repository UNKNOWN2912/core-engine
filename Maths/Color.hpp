#pragma once
#include <cstdint>
#include <glm/glm.hpp>
#include <string_view>

glm::vec4 HexColor4(uint32_t hexCode);
glm::vec3 HexColor3(uint32_t hexCode);

glm::vec4 HexColor4(std::string_view hexString);
glm::vec3 HexColor3(std::string_view hexString);

glm::vec4 RgbColor4(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
glm::vec3 RgbColor3(unsigned char r, unsigned char g, unsigned char b);
