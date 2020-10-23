#pragma once
#include "Math/rmg_Math.h"
#include <unordered_map>
#include <vector>

namespace rmg {

//Declarations of RMGraphics.cpp functions
bool Initialise(int argc, const char* argv[]);
void Cleanup();

//Styles
enum class FontAlign {
    Left = 0,
    Middle,
    Right
};
// enum class FontWrap {
//     Wrap = 0,   //Wrap text to next line if it overshoots
//     Clip        //Clip text so that it doesnt wrap to next line
// };

struct FontStyle
{
    FontStyle() = default;
    FontStyle(const FontStyle&) = default;
    FontStyle(FontAlign align, float lineSpacing);

    FontAlign Align;
    // FontWrap Wrap;
    float LineSpacing;  //Multiplier for current pixel sizeY
};
extern FontStyle FontStyleDefault;

class FontChar
{
public:
    FontChar() = default;
    FontChar(const FontChar&) = default;
    FontChar(unsigned int texid, unsigned int advance, const rmg::ivec2& size, const rmg::ivec2 bearing);
public:
    unsigned int TexId;
    unsigned int Advance;
    rmg::ivec2 Size;
    rmg::ivec2 Bearing;
};

class Font
{
    friend bool ::rmg::Initialise(int argc, const char* argv[]);
    friend void ::rmg::Cleanup();
    //Renderer

private:
    Font (void* pFace, float size);
    Font() = delete;
    Font(const Font&) = delete;
    Font& operator= (const Font&) = delete;

public:
    static Font* LoadFont (const char* strFontPath, float sizeY);
    void ReserveChars (const char* strChar);
    const FontChar* GetFontChar(int character);
    float GetSize();

private:
    bool LoadSingleChar(int character, std::vector<unsigned char>* pBuffer = nullptr);
    void Cleanup(); //Called automatically by the RMG lib inside OnLibCleanup

    static bool OnLibInit();
    static void OnLibCleanup();

private:
    void* m_pFontFace;
    std::unordered_map<int, FontChar> m_mapCharacters;
    float m_Size;
};
}