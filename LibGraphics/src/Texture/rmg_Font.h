#pragma once
#include "Math/rmg_Math.h"
#include <unordered_map>
#include <vector>

namespace rmg {

//Declarations of RMGraphics.cpp functions
bool Initialise(int argc, const char* argv[]);
void Cleanup();

//Declarations of Renderer

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
    const FontChar& GetFontChar(int character);
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