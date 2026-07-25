#include "FontImporter.hpp"
#include "Core/Macro.hpp"
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

    std::string printableCharacters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789{|}~:;<=>?@!\"#\\$%&'()*+,-./ ";

    Font font;
    for (char ch : printableCharacters)
    {
        error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
        if (error != FT_Err_Ok)
        {
            ERROR("Failed to load freetype char: {}", ch);
        }
        Glyph fontData;

        const FT_Bitmap &bitmap = face->glyph->bitmap;
        glm::uvec2 imageSize = glm::uvec2(bitmap.width, bitmap.rows);
        unsigned char *data = new unsigned char[size_t(imageSize.x * imageSize.y * 4)];

        if (ch != ' ')
        {
            fontData.textureId = TextureManager::CreateTexture(bitmap.buffer, imageSize, ImageFormat::R8UNORM, Filter::Nearest, Filter::Nearest);
        }
        else
        {
            unsigned char spaceData[4] = {0};
            fontData.textureId = TextureManager::CreateTexture(spaceData, {1, 1}, ImageFormat::R8UNORM, Filter::Nearest, Filter::Nearest);
        }

        FT_Outline outline = face->glyph->outline;
        unsigned short contourStart = 0;

        for (unsigned short i = 0; i < outline.n_contours; i++)
        {
            Contour contour;
            for (unsigned short j = contourStart; j <= outline.contours[i]; j++)
            {
                ContourPoint point;
                point.position = {outline.points[j].x / 64, outline.points[j].y / 64};

                if (outline.tags[j] == FT_CURVE_TAG_ON)
                {
                    point.control = ContourPointType::On;
                }
                else if (outline.tags[j] == FT_CURVE_TAG_CUBIC)
                {
                    point.control = ContourPointType::Cubic;
                }
                else if (outline.tags[j] == FT_CURVE_TAG_CONIC)
                {
                    point.control = ContourPointType::Quadratic;
                }

                contour.points.push_back(point);
            }
            fontData.contours.push_back(contour);

            contourStart = outline.contours[i];
        }

        fontData.advance = {(float)face->glyph->advance.x / 64, (float)face->glyph->advance.y / 64};
        fontData.bearing = {face->glyph->bitmap_left, face->glyph->bitmap_top};
        fontData.size = {face->glyph->bitmap.width, face->glyph->bitmap.rows};
        fontData.pixelSize = float(size);

        std::shared_ptr<Texture> texture = TextureManager::GetTexture(fontData.textureId);
        texture->SetName(std::format("Font: {}", ch));
        font.SetGlyphData(ch, fontData);
    }

    font.SetName(filename);

    return font;
}

void FontImporter::Initialize()
{
    mInitialize = true;

    FreeTypeManager::Initialize();
}

FT_Library FreeTypeManager::mLibrary;