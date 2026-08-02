#pragma once
#include "Assets/TextureManager.hpp"
#include "Renderer/StorageBuffer.hpp"
#include <unordered_map>

struct BezierCurve
{
    glm::vec2 start = glm::vec2(0);
    glm::vec2 control = glm::vec2(0);
    glm::vec2 end = glm::vec2(0);
};

struct Contour
{
    uint32_t startIndex = UINT32_MAX;
    uint32_t count = 0;
};

struct Glyph
{
    glm::vec2 advance;
    glm::vec2 bearing;
    glm::vec2 size;

    glm::vec2 max = glm::vec2(FLT_MIN);
    glm::vec2 min = glm::vec2(FLT_MAX);

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

    const std::vector<BezierCurve> &GetBezierCurve() const
    {
        return mCurves;
    }

    const StorageBuffer &GetStorageBuffer() const
    {
        return mStorageBuffer;
    }

    float GetMaxHeight() const
    {
        return mMaxHeight;
    }

private:
    std::unordered_map<char, Glyph> mCharaterImageMap;
    std::vector<BezierCurve> mCurves;
    std::string mName;
    float mFontSize = 0.f;
    float mMaxHeight = FLT_MIN;

    StorageBuffer mStorageBuffer;

    friend class FontImporter;
};