#pragma once
#include "Assets/Font.hpp"
#include "Assets/FontImporter.hpp"
#include <cstdint>
#include <string_view>

enum class FontID : uint64_t;
#define INVALID_FONT_ID FontID(UINT64_MAX)

class FontManager
{
public:
    static FontID Load(std::string_view filename, uint32_t fontSize);
    static void Destroy(FontID id);
    static const std::unordered_map<FontID, Font> &GetMap();
    static const Font &GetFont(FontID id);
    static bool HasFont(FontID id);
    static void Clear();
    static FontID GenerateID();

private:
    static std::unordered_map<FontID, Font> mFontMap;
    static FontImporter mImporter;
    static uint64_t mLastId;
};