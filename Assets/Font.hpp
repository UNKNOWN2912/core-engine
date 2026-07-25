#pragma once
#include "Assets/TextureManager.hpp"
#include <unordered_map>

enum class ContourPointType
{
    On,
    Quadratic,
    Cubic
};

struct ContourPoint
{
    glm::vec2 position = {};
    ContourPointType control = ContourPointType::On;
};

struct Contour
{
    std::vector<ContourPoint> points;
};

struct Glyph
{
    TextureID textureId = (TextureID)UINT64_MAX;

    float pixelSize;
    glm::vec2 advance;
    glm::vec2 bearing;
    glm::vec2 size;

    std::vector<Contour> contours;
};

class Font
{
public:
    const Glyph &GetGlyph(char ch) const
    {
        return mCharaterImageMap.at(ch);
    }

    void SetGlyphData(char ch, const Glyph &data)
    {
        mCharaterImageMap[ch] = data;
    }

    void SetName(std::string_view name)
    {
        mName = name;
    }

    const std::string &GetName() const
    {
        return mName;
    }

private:
    std::unordered_map<char, Glyph> mCharaterImageMap;
    float mFontSize = 0.f;
    std::string mName;
};