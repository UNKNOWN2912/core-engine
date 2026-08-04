#pragma once
#include "Assets/Font.hpp"
#include "Assets/FontImporter.hpp"
#include <cstdint>
#include <string_view>

class FontManager
{
public:
    static std::string Load(std::string_view filename, std::string_view identifier);
    static void Destroy(std::string_view identifier);
    static const std::unordered_map<std::string, Font> &GetMap();
    static const Font &GetFont(std::string_view identifier);
    static bool HasFont(std::string_view identifier);
    static void Clear();

private:
    static std::unordered_map<std::string, Font> mFontMap;
    static FontImporter mImporter;
    static uint64_t mLastId;
};