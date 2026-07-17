#pragma once
#include <unordered_map>

struct FontData
{
};

class Font
{
public:
    const FontData &GetImage(char ch) const
    {
        return mCharaterImageMap.at(ch);
    }

    void SetTexture(char ch, const FontData &data)
    {
        mCharaterImageMap[ch] = data;
    }

private:
    std::unordered_map<char, FontData> mCharaterImageMap;
};