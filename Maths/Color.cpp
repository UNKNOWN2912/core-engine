#include "Color.hpp"

glm::vec4 HexColor4(uint32_t hexCode)
{
    float r = float((hexCode & 0xFF000000) >> 24) / 255.f;
    float g = float((hexCode & 0x00FF0000) >> 16) / 255.f;
    float b = float((hexCode & 0x0000FF00) >> 8) / 255.f;
    float a = float((hexCode & 0x000000FF) >> 0) / 255.f;

    return {r, g, b, a};
}

glm::vec3 HexColor3(uint32_t hexCode)
{
    float r = float((hexCode & 0xFF0000) >> 16) / 255.f;
    float g = float((hexCode & 0x00FF00) >> 8) / 255.f;
    float b = float((hexCode & 0x0000FF) >> 0) / 255.f;

    return {r, g, b};
}

uint32_t getHexValue(char hexDigit)
{
    std::pair<char, uint32_t> hexDecimalPair[] =
        {
            {'0', 0},
            {'1', 1},
            {'2', 2},
            {'3', 3},
            {'4', 4},
            {'5', 5},
            {'6', 6},
            {'7', 7},
            {'8', 8},
            {'9', 9},
            {'A', 10},
            {'B', 11},
            {'C', 12},
            {'D', 13},
            {'E', 14},
            {'F', 15},
            {'a', 10},
            {'b', 11},
            {'c', 12},
            {'d', 13},
            {'e', 14},
            {'f', 15},
        };

    for (const auto &pair : hexDecimalPair)
    {
        if (pair.first == hexDigit)
        {
            return pair.second;
        }
    }

    return 0;
}

uint32_t getHexIntValue(char hex[2])
{
    return getHexValue(hex[1]) + (getHexValue(hex[0]) * 16);
}

glm::vec4 HexColor4(std::string_view hexString)
{
    assert(hexString.size() == 9 || hexString.size() == 7);
    char rComponent[2] = {hexString[1], hexString[2]};
    uint32_t rInt = getHexIntValue(rComponent);
    float r = float(rInt) / 255.f;

    char gComponent[2] = {hexString[3], hexString[4]};
    uint32_t gInt = getHexIntValue(gComponent);
    float g = float(gInt) / 255.f;

    char bComponent[2] = {hexString[5], hexString[6]};
    uint32_t bInt = getHexIntValue(bComponent);
    float b = float(bInt) / 255.f;

    float a = 0;

    if (hexString.size() == 9)
    {
        char aComponent[2] = {hexString[7], hexString[8]};
        uint32_t aInt = getHexIntValue(aComponent);
        a = float(aInt) / 255.f;
    }

    return {r, g, b, a};
}

glm::vec3 HexColor3(std::string_view hexString)
{
    assert(hexString.size() == 7);
    char rComponent[2] = {hexString[1], hexString[2]};
    char gComponent[2] = {hexString[3], hexString[4]};
    char bComponent[2] = {hexString[5], hexString[6]};

    float r = float(getHexIntValue(rComponent)) / 255.f;
    float g = float(getHexIntValue(gComponent)) / 255.f;
    float b = float(getHexIntValue(bComponent)) / 255.f;

    return {r, g, b};
}

glm::vec4 RgbColor4(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    float rn = float(r) / 255.f;
    float gn = float(g) / 255.f;
    float bn = float(b) / 255.f;
    float an = float(a) / 255.f;

    return {rn, gn, bn, an};
}

glm::vec3 RgbColor3(unsigned char r, unsigned char g, unsigned char b)
{
    float rn = float(r) / 255.f;
    float gn = float(g) / 255.f;
    float bn = float(b) / 255.f;

    return {rn, gn, bn};
}
