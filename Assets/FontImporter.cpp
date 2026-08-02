#include "FontImporter.hpp"
#include "Core/Macro.hpp"
#include "ftoutln.h"
#include <freetype.h>

class FreeTypeManager
{
public:
    static void Initialize()
    {
        FT_Error error = FT_Init_FreeType(&mLibrary);

        if (error)
        {
            ERROR("Failed to initialize freetype");
        }
    }

    static FT_Library GetFtLibrary()
    {
        return mLibrary;
    }

    static void Terminate()
    {
        FT_Done_FreeType(mLibrary);
    }

private:
    static FT_Library mLibrary;
};

struct FreetypeUserData
{
    uint32_t fontSize = 0;
    glm::vec2 start = glm::vec2(0);
    std::vector<Contour> *contours = nullptr;
    std::vector<BezierCurve> *bezier = nullptr;
    Contour *currentContour = nullptr;
    Font *font = nullptr;
    glm::vec2 min = glm::vec2(FLT_MAX);
    glm::vec2 max = glm::vec2(FLT_MIN);
};

glm::vec2 FtVector2ToGlmVec2(const FT_Vector *vec)
{
    return {vec->x, vec->y};
}

int moveTo(const FT_Vector *to, void *user)
{
    FreetypeUserData &data = *(FreetypeUserData *)user;
    glm::vec2 t = {to->x / 64, to->y / 64};
    t /= data.fontSize;
    t.y = 1 - t.y;

    data.currentContour = &data.contours->emplace_back();
    data.currentContour->startIndex = data.bezier->size();
    data.start = t;

    return 0;
}
int lineTo(const FT_Vector *to, void *user)
{
    FreetypeUserData &data = *(FreetypeUserData *)user;
    glm::vec2 t = {to->x / 64, to->y / 64};
    t /= data.fontSize;
    t.y = 1 - t.y;

    BezierCurve curve;
    curve.start = data.start;
    curve.end = t;
    curve.control = glm::mix(curve.start, curve.end, 0.5);

    data.bezier->push_back(curve);
    data.currentContour->count++;
    data.start = curve.end;

    data.min = glm::min(data.min, curve.start);
    data.min = glm::min(data.min, curve.end);

    data.max = glm::max(data.max, curve.start);
    data.max = glm::max(data.max, curve.end);

    return 0;
}
int conicTo(const FT_Vector *control, const FT_Vector *to, void *user)
{
    FreetypeUserData &data = *(FreetypeUserData *)user;
    glm::vec2 t = {to->x / 64, to->y / 64};
    t /= data.fontSize;
    t.y = 1 - t.y;
    glm::vec2 c = {control->x / 64, control->y / 64};
    c /= data.fontSize;
    c.y = 1 - c.y;

    BezierCurve curve;
    curve.start = data.start;
    curve.end = t;
    curve.control = c;

    data.bezier->push_back(curve);
    data.currentContour->count++;
    data.start = curve.end;

    data.min = glm::min(data.min, curve.start);
    data.min = glm::min(data.min, curve.end);

    data.max = glm::max(data.max, curve.start);
    data.max = glm::max(data.max, curve.end);

    return 0;
}
int cubicTo(const FT_Vector *control1, const FT_Vector *control2, const FT_Vector *to, void *user)
{
    FreetypeUserData &data = *(FreetypeUserData *)user;
    glm::vec2 t = {to->x / 64, to->y / 64};
    glm::vec2 c1 = {control1->x / 64, control1->y / 64};
    glm::vec2 c2 = {control2->x / 64, control2->y / 64};

    t /= data.fontSize;
    c1 /= data.fontSize;
    c2 /= data.fontSize;

    t.y = 1 - t.y;
    c1.y = 1 - c1.y;
    c2.y = 1 - c2.y;

    return 0;
}

Font FontImporter::Import(std::string_view filename, uint32_t size)
{
    if (!mInitialize)
    {
        Initialize();
    }

    FT_Face face = nullptr;
    FT_Error error = FT_New_Face(FreeTypeManager::GetFtLibrary(), filename.data(), 0, &face);
    if (error != FT_Err_Ok)
    {
        ERROR("Failed to open font file: {}", filename);
    }

    error = FT_Set_Pixel_Sizes(face, 0, size);
    if (error != FT_Err_Ok)
    {
        ERROR("Failed to set freetype size");
    }

    std::string printableCharacters = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~ ";

    Font font;
    for (char ch : printableCharacters)
    {
        error = FT_Load_Char(face, ch, FT_LOAD_DEFAULT);
        if (error != FT_Err_Ok)
        {
            ERROR("Failed to load freetype char: {}", ch);
        }
        Glyph fontData;

        FreetypeUserData userData;
        userData.font = &font;
        userData.contours = &fontData.contours;
        userData.bezier = &font.mCurves;
        userData.fontSize = size;

        FT_Outline_Funcs functions =
            {
                .move_to = moveTo,
                .line_to = lineTo,
                .conic_to = conicTo,
                .cubic_to = cubicTo,
                .shift = 0,
                .delta = 0,
            };

        FT_Outline_Decompose(&face->glyph->outline, &functions, (void *)&userData);

        fontData.advance = {(float)face->glyph->advance.x / 64, (float)face->glyph->advance.y / 64};
        fontData.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
        fontData.size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        fontData.min = userData.min;
        fontData.max = userData.max;

        font.mMaxHeight = glm::max(font.mMaxHeight, fontData.max.y);

        fontData.advance /= size;
        fontData.bearing /= size;
        fontData.size /= size;

        for (int i = 0; i < fontData.contours.size(); i++)
        {
            const Contour &contour = fontData.contours[i];
            for (uint32_t j = contour.startIndex; j < contour.startIndex + contour.count; j++)
            {
                BezierCurve &curve = font.mCurves[j];

                curve.control -= fontData.min;
                curve.end -= fontData.min;
                curve.start -= fontData.min;

                curve.control /= fontData.size;
                curve.end /= fontData.size;
                curve.start /= fontData.size;
            }
        }

        font.SetGlyphData(ch, fontData);
    }

    font.mStorageBuffer.CreateStorageBuffer(font.mCurves.data(), font.mCurves.size() * sizeof(BezierCurve));

    font.SetName(filename);

    return font;
}

void FontImporter::Initialize()
{
    mInitialize = true;

    FreeTypeManager::Initialize();
}

FT_Library FreeTypeManager::mLibrary;