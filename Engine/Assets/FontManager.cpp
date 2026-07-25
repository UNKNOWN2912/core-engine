#include "FontManager.hpp"

FontID FontManager::Load(std::string_view filename, uint32_t fontSize)
{
    FontID id = GenerateID();
    Font font = mImporter.Import(filename, fontSize);
    mFontMap[id] = font;
    return id;
}
void FontManager::Destroy(FontID id)
{
}
const std::unordered_map<FontID, Font> &FontManager::GetMap()
{
    return mFontMap;
}

const Font &FontManager::GetFont(FontID id)
{
    return mFontMap.at(id);
}

bool FontManager::HasFont(FontID id)
{
    return mFontMap.contains(id);
}

void FontManager::Clear()
{
    mFontMap.clear();
}

FontID FontManager::GenerateID()
{
    return (FontID)mLastId++;
}

std::unordered_map<FontID, Font> FontManager::mFontMap;
FontImporter FontManager::mImporter;
uint64_t FontManager::mLastId;