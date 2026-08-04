#include "FontManager.hpp"

std::string FontManager::Load(std::string_view filename, std::string_view identifier)
{
    Font font = mImporter.Import(filename, 1024);
    mFontMap[identifier.data()] = font;
    return identifier.data();
}

void FontManager::Destroy(std::string_view identifier)
{
}

const std::unordered_map<std::string, Font> &FontManager::GetMap()
{
    return mFontMap;
}

const Font &FontManager::GetFont(std::string_view identifier)
{
    return mFontMap[identifier.data()];
}

bool FontManager::HasFont(std::string_view identifier)
{
    return mFontMap.contains(identifier.data());
}

void FontManager::Clear()
{
    mFontMap.clear();
}

std::unordered_map<std::string, Font> FontManager::mFontMap;
FontImporter FontManager::mImporter;
uint64_t FontManager::mLastId;