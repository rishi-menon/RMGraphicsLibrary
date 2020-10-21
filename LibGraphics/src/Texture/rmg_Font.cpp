#include "Texture/rmg_Font.h"
#include "Texture/rmg_Texture.h"
#include "Texture/rmg_Texture_int.h"
#include "rmg_Constants.h"
#include "rmg_Constants_int.h"
#include "Log/rmg_Log_int.h"
#include "Log/rmg_Log.h"
#include <list>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace rmg {
    FontChar::FontChar(unsigned int texid, unsigned int advance, const rmg::ivec2& size, const rmg::ivec2 bearing) :
        TexId (texid),
        Advance (advance),
        Size (size),
        Bearing (bearing)
    {

    }
}

namespace rmg {
    static FT_Library s_ftLib;
    bool s_initialised = false;
    std::list<Font*> s_listFonts;

    Font::Font(void* pFace, float size) :
        m_pFontFace(pFace),
        m_Size (size)
    {
    }

    float Font::GetSize () { return m_Size;} 
    
    void Font::ReserveChars (const char* strChar)
    {
        std::list<int> listLoadChars;
        for (int i = 0; strChar[i] != '\0'; i++)
        {
            int c = (int)strChar[i];
            if (m_mapCharacters.find(c) == m_mapCharacters.end())
            {
                listLoadChars.push_back (c);
            }
        }

        if (listLoadChars.empty()) { return; }
        
        std::vector<unsigned char> buffer;
        for (int i : listLoadChars)
        {
            LoadSingleChar(i, &buffer);
        }
    }
    void Font::Cleanup()
    {
        if (m_pFontFace)
        {
            FT_Face* pFace = (FT_Face*)(m_pFontFace);
            if (FT_Done_Face (*pFace))
            {
                RMG_LOG_IWARN ("RMG Warning: Could not free font face");
            }
            delete pFace;

            m_mapCharacters.clear();
        }
    }

    const FontChar& Font::GetFontChar(int character)
    {
        std::unordered_map<int, FontChar>::iterator it = m_mapCharacters.find(character);
        if (it != m_mapCharacters.end())
        {
            return it->second;
        }
        else
        {
            //We need to load the character and then return it
            if (LoadSingleChar (character))
            {
                
                return m_mapCharacters.at (character);
            }
            else
            {
                IASSERT (false, "");
                //This is bad... We couldn't create the character that we were requesting
                return m_mapCharacters.begin()->second; //return an arbitrary character
            }
        }
    }
    bool Font::LoadSingleChar(int character, std::vector<unsigned char>* pBuffer)
    {
        FT_Face* pFace = (FT_Face*)m_pFontFace;
        if (FT_Load_Char (*pFace, character, FT_LOAD_RENDER))
        {
            IASSERT(false, "RMG Error: Could not load font character: {0} ({1})", character, (char)(character));
            return false;
        }

        std::size_t nWidth = (*pFace)->glyph->bitmap.width;
        std::size_t nHeight = (*pFace)->glyph->bitmap.rows;

        std::vector<unsigned char> bufferBackup;
        if (!pBuffer)  pBuffer = &bufferBackup;

        std::size_t nTotalLength = nWidth * nHeight * 4;
        pBuffer->reserve (nTotalLength);
        pBuffer->clear();
        for (std::size_t i = 0; i < nTotalLength; i++)
		{
            pBuffer->emplace_back (255);
		}

        //The buffer we get from FT consists of only an alpha channel... To use the same shader, we convert this into an rgba buffer... Start with a buffer with all 255 and set the alpha of each pixel to that of the font character.. Hence start from an index of 3 and increment by 4
        unsigned char* pOriginalBuffer = (*pFace)->glyph->bitmap.buffer;
        std::size_t nIndex = 3; //Alpha channel

        for (std::size_t i = 0; i < nHeight; i++)
        {
            std::size_t nRowIndexBackwards = (nHeight - i - 1) * nWidth; 
            for (std::size_t j = 0; j < nWidth; j++)
            {
                (*pBuffer)[nIndex] = pOriginalBuffer[nRowIndexBackwards + j];  //We need to reverse the order of the buffer
                nIndex += 4;    //Keep assigning to alpha channels
            }
        }

        FontChar fontchar;
        fontchar.TexId = Texture::LoadTextureBuffer (pBuffer->data(), nWidth, nHeight);
        fontchar.Advance = (*pFace)->glyph->advance.x >> 6;    //advance is in 1/64th of a pixel
        fontchar.Size = rmg::ivec2(nWidth, nHeight);
        fontchar.Bearing.x = (*pFace)->glyph->bitmap_left;
        fontchar.Bearing.y = (*pFace)->glyph->bitmap_top;

        m_mapCharacters.emplace (character, fontchar);
        RMG_LOG_IINFO (m_mapCharacters.size());
    }

//Static methods
//return nullptr if it fails
    Font* Font::LoadFont (const char* strFontPath, float sizeY)
    {
        std::string strFullPath = Texture::GetCWD() + strFontPath;

        FT_Face* pface = new FT_Face();
        FT_Error err = FT_New_Face (s_ftLib, strFullPath.c_str(), 0, pface);
        if (err)
        {
            IASSERT (false, "RMG Error: FT_ERROR {0} Could not load font: {1}", err, strFontPath);
            return nullptr;
        }

        FT_Set_Pixel_Sizes(*pface, 0, sizeY);

        Font* pf = new(std::nothrow) Font((void*)(pface), sizeY);
        if (pf)
        {
            s_listFonts.push_back (pf);
        }

        return pf;
    }
    bool Font::OnLibInit()
    {
        if (s_initialised)
        {
            RMG_LOG_IWARN ("Font has already been initialised");
            return false;
        }
        if (FT_Init_FreeType (&s_ftLib))
        {
            IASSERT (false, "RMG Error: Could not initialise Font library");
            return false;
        }
        s_initialised = true;
        return true;
    }
    void Font::OnLibCleanup()
    {
        for (Font* pFont : s_listFonts)
        {
            if (pFont)
            {
                pFont->Cleanup();
                delete pFont;
            }
        }
        FT_Done_FreeType (s_ftLib);
    }
}