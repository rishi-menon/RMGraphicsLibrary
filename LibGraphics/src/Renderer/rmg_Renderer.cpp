#include "rmg_Renderer.h"
#include "rmg_Renderer_int.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h> //Might not need this
#include "Math/rmg_Math.h"
#include "Renderer/rmg_RendererVertex_int.h"
#include "Log/rmg_Log_int.h"
#include "Log/rmg_Log.h"

#include "Input/rmg_Input.h"
#include "Input/rmg_Input_int.h"

#include "Texture/rmg_Texture.h"
#include "Texture/rmg_Texture_int.h"

#include "Renderer/rmg_RendererShapes_int.h"
#include "Texture/rmg_Font.h"

static unsigned int CompileShader(unsigned int nShaderType, const char* strShaderCode)
{
    unsigned int nId = glCreateShader (nShaderType);
    rmg_glcall(glShaderSource (nId, 1, &strShaderCode, 0));
    rmg_glcall(glCompileShader(nId));

    int nStatus = 0;
    rmg_glcall(glGetShaderiv (nId, GL_COMPILE_STATUS, &nStatus));

    if (!nStatus)
    {
        int size;
        rmg_glcall(glGetShaderiv (nId, GL_INFO_LOG_LENGTH, &size));
        char* buffer = new char[size];
        if (buffer)
        {
            glGetShaderInfoLog (nId, size, nullptr, buffer);
            const char* strShaderType = (nShaderType == GL_VERTEX_SHADER) ? "Vertex" : "Fragment";
            RMG_LOG_IERROR ("RMG Error: Failed to compile {0} shader: {1}", strShaderType, buffer);
            IASSERT (false, "");
            delete[] buffer;

        }
        return (unsigned int) (-1);
    }
    return nId;
}
static const char* s_strShaderVertex = R"(
#version 330 core

layout(location = 0) in vec4 a_pos;
layout(location = 1) in vec4 a_col;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in float a_texId;

out vec4 v_col;
out vec2 v_texCoord;
out float v_texId;

uniform mat4 u_mvp;
void main() {
    gl_Position = u_mvp * a_pos;
    v_col = a_col;
    v_texCoord = a_texCoord;
    v_texId = a_texId;
}
)";

static const char* s_strShaderFragmentBase = R"(
#version 330 core

layout(location = 0) out vec4 col;

in vec4 v_col;
in vec2 v_texCoord;
in float v_texId;
uniform sampler2D u_textureSlots[%d];

void main()
{
    int texId = int(v_texId);
    col = v_col * texture(u_textureSlots[texId], v_texCoord);
}
)";


//Constants
static const unsigned int s_nMaxVertex = 1000; 
static const unsigned int s_nMaxIndex = 1000*6/4;
static uint8_t s_nMaxTextureSlots = 16;

//Props
static unsigned int s_nVao;
static unsigned int s_nVbo;
static unsigned int s_nIbo;
static unsigned int s_nShader;
static int s_nMvpLocation = -1;

static uint8_t s_nCurTexIndex = 1;  //0 is reserved for white color texture
static uint32_t s_nCurVertexCount = 0;
static uint32_t s_nCurIndexCount = 0;

static rmg::RendererVertex* s_vertexArray = nullptr;
static unsigned int* s_indexArray = nullptr;
static unsigned int* s_boundTexSlots = nullptr;

static unsigned int s_nWhiteTextureId;

static rmg::Font* s_pDefaultFont = nullptr;

namespace rmg {
namespace Renderer {

    bool Init ()
    {
        if (s_vertexArray || s_indexArray)
        {
            RMG_LOG_IWARN ("RMG Warning: Renderer has already been initialised");
            return false;
        }
        //Set total texture slots
        {
            int nTotalTexSlots;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nTotalTexSlots);
            s_nMaxTextureSlots = nTotalTexSlots;
        }

        //Start Createing buffers
        rmg_glcall(glGenVertexArrays (1, &s_nVao));
        rmg_glcall(glBindVertexArray (s_nVao));

        //Create and bind Vertex buffer
        rmg_glcall(glGenBuffers (1, &s_nVbo));
        rmg_glcall(glBindBuffer (GL_ARRAY_BUFFER, s_nVbo));
        rmg_glcall(glBufferData (GL_ARRAY_BUFFER, sizeof(RendererVertex) * s_nMaxVertex, nullptr, GL_DYNAMIC_DRAW));
        //Create and bind index buffer
        rmg_glcall(glGenBuffers (1, &s_nIbo));
        rmg_glcall(glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, s_nIbo));
        rmg_glcall(glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * s_nMaxIndex, nullptr, GL_DYNAMIC_DRAW));
        
        //Specify Layout for Vertex buffer
        rmg_glcall(glEnableVertexAttribArray (0));
        rmg_glcall(glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, sizeof (RendererVertex), (const void*)offsetof(RendererVertex, m_vPos)));

        rmg_glcall(glEnableVertexAttribArray (1));
        rmg_glcall(glVertexAttribPointer (1, 4, GL_FLOAT, GL_FALSE, sizeof (RendererVertex), (const void*)offsetof(RendererVertex, m_vCol)));

        rmg_glcall(glEnableVertexAttribArray (2));
        rmg_glcall(glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, sizeof (RendererVertex), (const void*)offsetof(RendererVertex, m_vTexCoords)));

        rmg_glcall(glEnableVertexAttribArray (3));
        rmg_glcall(glVertexAttribPointer (3, 1, GL_FLOAT, GL_FALSE, sizeof (RendererVertex), (const void*)offsetof(RendererVertex, m_fTexId)));

        //Shader
        //First create the fragment shader from the template
        char* strFragmentShader;
        {
            //+10 just to have some extra
            std::size_t fragLen = strlen (s_strShaderFragmentBase) + 10;    
            strFragmentShader = new(std::nothrow) char[fragLen];
            if (!strFragmentShader)
            {
                IASSERT (false, "RMG Error: Could not allocate enough heap memory for fragment shader");
                return false;
            }
            std::snprintf (strFragmentShader, fragLen, s_strShaderFragmentBase, s_nMaxTextureSlots);
        }

        s_nShader = glCreateProgram ();
        unsigned int nVertex = CompileShader(GL_VERTEX_SHADER, s_strShaderVertex);
        unsigned int nFragment = CompileShader(GL_FRAGMENT_SHADER, strFragmentShader);
        
        if (nVertex == (unsigned int)(-1) || nFragment == (unsigned int)(-1)) 
        {   
            //cleanup
            delete[] strFragmentShader;
            strFragmentShader = nullptr;
            return false;
        }
        rmg_glcall(glAttachShader (s_nShader, nVertex));
        rmg_glcall(glAttachShader (s_nShader, nFragment));
        rmg_glcall(glLinkProgram(s_nShader));

        int nLinkStatus = 0;
        rmg_glcall(glGetProgramiv (s_nShader, GL_LINK_STATUS, &nLinkStatus));
        if (!nLinkStatus)
        {
            constexpr int len = 1000;
            char* buff = new char[len];
            glGetProgramInfoLog (s_nShader, len, nullptr, buff);
            RMG_LOG_IERROR ("RMG Error: Could not link the shader: {0}", buff);
            delete[] buff;
            IASSERT (false, "");

            //cleanup
            delete[] strFragmentShader;
            strFragmentShader = nullptr;
            return false;
        }
        rmg_glcall(glUseProgram (s_nShader));

        //cleanup
        delete[] strFragmentShader;
        strFragmentShader = nullptr;
        

        //Set uniforms
        int nTexId = glGetUniformLocation (s_nShader, "u_textureSlots");
        if (nTexId == -1)
        {
            RMG_LOG_IWARN ("RMG Warning: Uniform u_textureSlots was not found");
            return false;
        }
        else
        {
            int uTexSlots[32];
            if (s_nMaxTextureSlots > 32)
            {
                RMG_LOG_IERROR("RMG Error: Cannot have more than 32 texture slots");
                return false;
            }

            for (int i = 0; i < (int)s_nMaxTextureSlots; uTexSlots[i] = i, i++);
            glUniform1iv (nTexId, s_nMaxTextureSlots, uTexSlots);
        }
        
        s_nMvpLocation = glGetUniformLocation (s_nShader, "u_mvp");
        if (s_nMvpLocation == -1)
        {
            RMG_LOG_IWARN ("RMG Warning: Uniform u_mvp was not found");
            return false;
        }

        //Load a white texture
        unsigned char whiteRgba[4] = {255, 255, 255, 255};
        s_nWhiteTextureId = Texture::LoadTextureBuffer (whiteRgba, 1, 1);

        s_vertexArray = new(std::nothrow) RendererVertex[s_nMaxVertex];
        s_indexArray = new(std::nothrow) unsigned int[s_nMaxIndex];
        s_boundTexSlots = new(std::nothrow) unsigned int[s_nMaxTextureSlots];

        if (!s_vertexArray || !s_indexArray || !s_boundTexSlots)
        {
            RMG_LOG_IERROR ("RMG Error: Could not allocate enough heap memory for the renderer");
            return false;
        }

        for (unsigned int i = 1; i < s_nMaxTextureSlots; i++)
        {
            s_boundTexSlots[i] = (unsigned int)(-1);    //Reset to an invalid state
        }
        s_boundTexSlots[0] = s_nWhiteTextureId; //0 is reserved for white texture

        return true;
    }

    bool SetDefaultFont (const char* strFontPath, int nSizeY)
    {
        s_pDefaultFont = Font::LoadFont (strFontPath, nSizeY);
        return (s_pDefaultFont != nullptr);
    }

    void Flush ()
    {
        if (!s_nCurVertexCount || !s_nCurIndexCount)
        {
            return;
        }

        //Bind the white texture just in case Texture::LoadTexture caused it to unbind
        rmg_glcall(glActiveTexture (GL_TEXTURE0));
        rmg_glcall(glBindTexture (GL_TEXTURE_2D, s_nWhiteTextureId));

        rmg_glcall(glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(RendererVertex) * s_nCurVertexCount, s_vertexArray));

        rmg_glcall(glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * s_nCurIndexCount, s_indexArray));

        rmg_glcall(glDrawElements (GL_TRIANGLES, s_nCurIndexCount, GL_UNSIGNED_INT, 0));

        s_nCurIndexCount = 0;
        s_nCurVertexCount = 0;
        s_nCurTexIndex = 1; //0 is reserved for white 
        
        //reset the tex slots
        for (uint8_t i = 0; i < s_nMaxTextureSlots; s_boundTexSlots[i] = (unsigned int)(-1), i++);
        
        s_boundTexSlots[0] = s_nWhiteTextureId;
    }

    unsigned int GetWhiteTexId () { return s_nWhiteTextureId; }

    void DrawGeneric (const RendererVertex* vertexBuffer, RendererShapes::Shapes shape, unsigned int nTexId)
    {
        DrawGeneric(vertexBuffer, RendererShapes::GetVertexCount(shape),
        RendererShapes::GetIndexBuffer(shape), RendererShapes::GetIndexCount(shape), nTexId);
    }
    void DrawGenericColor(const RendererVertex* vertexBuffer, RendererShapes::Shapes shape)
    {
        DrawGeneric(vertexBuffer, RendererShapes::GetVertexCount(shape),
        RendererShapes::GetIndexBuffer(shape), RendererShapes::GetIndexCount(shape), s_nWhiteTextureId);
    }


    void DrawGenericColor (const RendererVertex* vertexBuffer, unsigned int nVertexCount, unsigned int* indexBuffer, unsigned int nIndexCount)
    {
        DrawGeneric (vertexBuffer, nVertexCount, indexBuffer, nIndexCount, s_nWhiteTextureId);
    }

    void DrawGeneric (const RendererVertex* vertexBuffer, unsigned int nVertexCount, unsigned int* indexBuffer, unsigned int nIndexCount, unsigned int nTexId)
    {

        if ( s_nCurIndexCount + nIndexCount > s_nMaxIndex || 
             s_nCurVertexCount + nVertexCount > s_nMaxVertex)
        {
            //Not enough room in the current batch
            Renderer::Flush();
        }

        bool bFound = false;
        unsigned int nTextureSlot;
        for (int i = 0; i < s_nMaxTextureSlots; i++)
        {
            if (s_boundTexSlots[i] == nTexId)
            {
                nTextureSlot = i;  //Texture is already bound in the current batch
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            if (s_nCurTexIndex >= s_nMaxTextureSlots)
            {
                //No more room in current batch
                Renderer::Flush();
            }

            rmg_glcall(glActiveTexture (GL_TEXTURE0 + s_nCurTexIndex));
            rmg_glcall(glBindTexture (GL_TEXTURE_2D, nTexId));
            s_boundTexSlots[s_nCurTexIndex] = nTexId;
            nTextureSlot = s_nCurTexIndex;
            ++s_nCurTexIndex;
        }

        //We have covered the safety checks where we dont have enough room in the current batch

        RendererVertex* vertexOffset = s_vertexArray + s_nCurVertexCount;
        for (unsigned int i = 0; i < nVertexCount; i++)
        {
            vertexOffset[i] = vertexBuffer[i];
            vertexOffset[i].SetTexId (nTextureSlot);  
        }

        unsigned int* indexOffsetPos = s_indexArray + s_nCurIndexCount;
        for (unsigned int i = 0; i < nIndexCount; i++)
        {
            indexOffsetPos[i] = s_nCurVertexCount + indexBuffer[i];
        }

        s_nCurIndexCount += nIndexCount;
        s_nCurVertexCount += nVertexCount;
    }


    ////////////////////////////////////////////////////////////////////////////////////
    ///////                    Text rendering functions
    ////////////////////////////////////////////////////////////////////////////////////

    static void GenerateQuadVertexBuffer (RendererVertex v[4], const rmg::vec3& pos, const rmg::vec2& size, const Color& col)
    {
        
        v[0].SetPropTex (pos, col, rmg::vec2 (0.0f, 0.0f));
        v[1].SetPropTex (rmg::vec3(pos.x + size.x, pos.y, pos.z), col, rmg::vec2 (1.0f, 0.0f));
        v[2].SetPropTex (rmg::vec3(pos.x + size.x, pos.y + size.y, pos.z), col, rmg::vec2 (1.0f, 1.0f));
        v[3].SetPropTex (rmg::vec3(pos.x, pos.y + size.y, pos.z), col, rmg::vec2 (0.0f, 1.0f));
    }

    ////////////////////////////////////////
    /////       Left Align
    ////////////////////////////////////////
    static void DrawTextLine_LeftAligned (const char* strText, int strLen, const rmg::vec3& posOriginal, float scale, const Color& color, const FontStyle& style, Font* pFont)
    {
        IASSERT (style.Align == FontAlign::Left, "RMG Error: Should be Left Aligned inside internal function");

        rmg::vec3 posQuad = posOriginal;    //posRect is different than the x position because each font character has a bearing X and bearing Y

        std::size_t i = 0;
        float posX;
        for (; i < strLen && strText[i] != '\0'; i++)
        {
            const FontChar* fc = pFont->GetFontChar(strText[i]);
            if (fc)  
            {
                //Offset it a bit to the left so that the first character will be printed exactly at the x coordinate specified in posistion instead of a few pixels away
                posX = posOriginal.x - fc->Bearing.x * scale;
                break;
            }
        }

        for (; i < strLen && strText[i] != '\0'; i++)
        {
            const FontChar* fc = pFont->GetFontChar(strText[i]);
            if (!fc)  continue;
            if (strText[i] != ' ')
            {
                posQuad.x = posX + fc->Bearing.x * scale;
                posQuad.y = posOriginal.y - (fc->Size.y - fc->Bearing.y) * scale;
                rmg::vec2 size = rmg::vec2(fc->Size.x * scale, fc->Size.y * scale);

                RendererVertex v[4];
                GenerateQuadVertexBuffer (v, posQuad, size, color);

                Renderer::DrawGeneric (v, RendererShapes::Shapes::Quad, fc->TexId);
            }
            posX += fc->Advance * scale;    //Update After

        }
    }

    ////////////////////////////////////////
    /////       Right Align
    ////////////////////////////////////////

    static void DrawTextLine_RightAligned (const char* strText, int strLen, const rmg::vec3& posOriginal, float scale, const Color& color, const FontStyle& style, Font* pFont)
    {
        IASSERT (style.Align == FontAlign::Right, "RMG Error: Should be Right Aligned inside internal function");

        //Offset it a bit to the left so that the first character will be printed exactly at the x coordinate specified in posistion instead of a few pixels away
        float posX;
        int i = strLen - 1;
        for (; i >= 0; i--) {
            const FontChar* fc = pFont->GetFontChar(strText[i]);
            if (fc)
            {
                posX = posOriginal.x + (fc->Advance - fc->Bearing.x - fc->Size.x)*scale;
                break;
            }
        }

        rmg::vec3 posQuad = posOriginal;    //posRect is different than the x position because each font character has a bearing X and bearing Y
        for (int i = strLen-1; i >= 0; i--)
        {
            const FontChar* fc = pFont->GetFontChar(strText[i]);
            if (!fc)  continue;

            posX -= fc->Advance * scale;    //Update Before
            if (strText[i] != ' ')
            {
                posQuad.x = posX + fc->Bearing.x * scale;
                posQuad.y = posOriginal.y - (fc->Size.y - fc->Bearing.y) * scale;
                rmg::vec2 size = rmg::vec2(fc->Size.x * scale, fc->Size.y * scale);

                RendererVertex v[4];
                GenerateQuadVertexBuffer (v, posQuad, size, color);

                Renderer::DrawGeneric (v, RendererShapes::Shapes::Quad, fc->TexId);
            }   
        }
    }

    ////////////////////////////////////////
    /////       Center Align
    ////////////////////////////////////////

    static void DrawTextLine_CenterAligned (const char* strText, int strLen, const rmg::vec3& posOriginal, float scale, const Color& color, const FontStyle& style, Font* pFont)
    {
        IASSERT (style.Align == FontAlign::Middle, "RMG Error: Should be Right Aligned inside internal function");

        FontStyle newStyle = style;
        int halfLen = strLen / 2;
        if (strLen % 2 == 0)
        {
            const FontChar* fcMiddleLeft = pFont->GetFontChar (strText[halfLen-1]);
            const FontChar* fcMiddleRight = pFont->GetFontChar (strText[halfLen]);

            rmg::vec3 posLeft = posOriginal;
            rmg::vec3 posRight = posOriginal;

            if (fcMiddleLeft && fcMiddleRight)
            {
                //calculate and add some padding between characters
                
                //Relative x position of the right edge of the middle left character. So for "ABCD" this would be the x value of the rightost edge of letter B.
                const float leftcharPos = fcMiddleLeft->Bearing.x + fcMiddleLeft->Size.x;
                //Relative x position of the left edge of letter C
                const float rightcharPos = fcMiddleLeft->Advance + fcMiddleRight->Bearing.x;
                float fPadding = (rightcharPos - leftcharPos) * scale;
                fPadding /= 2.0f;
                posLeft.x -= fPadding;
                posRight.x += fPadding; 
            }

            //Left half of the string should be right aligned
            newStyle.Align = FontAlign::Right;
            DrawTextLine_RightAligned (strText, halfLen, posLeft, scale, color, newStyle, pFont);
            //Right half of the string should be left aligned
            newStyle.Align = FontAlign::Left;
            DrawTextLine_LeftAligned (strText + halfLen, halfLen, posRight, scale, color, newStyle, pFont);
        }
        else
        {
            //Odd number of characters... Offset the position a bit so that its truely center aligned
            const FontChar* fcMiddleLeft = pFont->GetFontChar (strText[halfLen-1]);
            const FontChar* fcMiddle = pFont->GetFontChar (strText[halfLen]);

            rmg::vec3 posLeft = posOriginal;
            
            if (fcMiddle)  posLeft.x -= fcMiddle->Size.x * scale / 2.0f;
            
            rmg::vec3 posRight = posLeft;
            
            //calculate and add some padding between characters
            if (fcMiddleLeft && fcMiddle)
            {
                //Relative x position of the right edge of the middle left character. So for "ABCDE" this would be the x value of the rightost edge of letter B.
                const float leftcharPos = fcMiddleLeft->Bearing.x + fcMiddleLeft->Size.x;
                //Relative x position of the left edge of letter C
                const float rightcharPos = fcMiddleLeft->Advance + fcMiddle->Bearing.x;
                float fPadding = (rightcharPos - leftcharPos) * scale;
                fPadding /= 2.0f;
                posLeft.x -= fPadding;
                posRight.x += fPadding; 
            }

            //The middle character can either be drawn with the left half or the right half... In this implementation it is being drawn with the right half (for no particular reason)
            //Draw the left half which is right aligned
            newStyle.Align = FontAlign::Right;
            DrawTextLine_RightAligned (strText, halfLen, posLeft, scale, color, newStyle, pFont);
            //Draw the right half which is left aligned (also incldues center character)
            newStyle.Align = FontAlign::Left;
            DrawTextLine_LeftAligned (strText+halfLen, halfLen+1, posRight, scale, color, newStyle, pFont);
        }
    }

    static inline void DrawTextLine (const char* strText, int strLen, const rmg::vec3& posOriginal, float scale, const Color& color, const FontStyle& style, Font* pFont)
    {
        //pFont cannot be null here
        IASSERT (pFont, "RMG Error: Font was null");
        if (!strLen) return;

        switch (style.Align)
        {
            case FontAlign::Left:
            {
                DrawTextLine_LeftAligned (strText, strLen, posOriginal, scale, color, style, pFont);
                break;
            }
            case FontAlign::Right:
            {
                DrawTextLine_RightAligned (strText, strLen, posOriginal, scale, color, style, pFont);
                break;
            }
            case FontAlign::Middle:
            {
                DrawTextLine_CenterAligned (strText, strLen, posOriginal, scale, color, style, pFont);
                break;
            }
        }
    }

    void DrawText (const char* strText, const rmg::vec3& posOriginal, float pixelSize, const Color& color, const FontStyle& style, Font* pFont)
    {
        if (!pFont)
        {
            if (s_pDefaultFont)
            {
                 pFont = s_pDefaultFont; //Use default font instead
            }
            else
            {
                RMG_LOG_IWARN ("RMG Warning: Font and Default Font were both not provided. Please specify either of them. Cannot print text: {0}", strText);
                return;
            }
        }

        pFont->ReserveChars (strText);

        
        const float multiplier = 1.41f; 
        //GetSize returns the pixel height that was set while loading the font so when you divide pixelSize by that value you get the scaling factor... For some reason this pixel height is a bit smaller than the actual pixel height, and hence the multiplier
        float scale = multiplier * pixelSize / pFont->GetSize();    

        //Start printing 
        rmg::vec3 pos = posOriginal;
        bool bContinue = true;

        for (int start = 0, end = 0; bContinue; end++)
        {
            char c = strText[end];
            if (c == '\n' || c == '\0')
            {
                int len = end - start;  //Length will be excluding the \n character because there is no need to include that to print
                const char* strTextStart = (strText + start);
                DrawTextLine (strTextStart, len, pos, scale, color, style, pFont);

                end++;
                start = end;
                pos.y -= style.LineSpacing * pixelSize;
            }

            bContinue = (c != '\0');
        }
    }

    void Cleanup()
    {
        if (s_vertexArray)
        {
            delete[] s_vertexArray;
            s_vertexArray = nullptr;
        }
        if (s_indexArray)
        {
            delete[] s_indexArray;
            s_indexArray = nullptr;
        }
        if (s_boundTexSlots)
        {
            delete[] s_boundTexSlots;
            s_boundTexSlots = nullptr;
        }
    }

    void OnWindowResize (int width, int height)
    {
        if (s_nMvpLocation == -1)
        {
            RMG_LOG_IWARN ("RMG Warning: MVP matrix shader location was not set while resizing window");
        }
        else
        {
            rmg::mat4 matProj = glm::ortho<float> (0.0f, (float)width, 0.0f, (float)height, 1.0f, -1.0f);
            rmg_glcall(glUniformMatrix4fv (s_nMvpLocation, 1, GL_FALSE, &matProj[0][0]));
        }
        rmg_glcall(glViewport (0, 0, width, height));
        // rmg_glcall(glViewport (-1.0f, -1.0f, 1.0f, 1.0f));
    }


//////////////////////////////////////////////////////////////////////
///////                 Client side API                        ///////
//////////////////////////////////////////////////////////////////////

    void DrawQuadCol (const rmg::vec3& pos, const rmg::vec2& size, const Color& color)
    {
        RendererVertex v[4];
        rmg::vec2 halfSize = size / 2.0f;
        vec4 col = color.ToPercent();
        v[0].SetPropCol (vec3(pos.x - halfSize.x, pos.y - halfSize.y, pos.z), col);
        v[1].SetPropCol (vec3(pos.x + halfSize.x, pos.y - halfSize.y, pos.z), col);
        v[2].SetPropCol (vec3(pos.x + halfSize.x, pos.y + halfSize.y, pos.z), col);
        v[3].SetPropCol (vec3(pos.x - halfSize.x, pos.y + halfSize.y, pos.z), col);
        Renderer::DrawGenericColor (v, RendererShapes::Shapes::Quad);
    }
    void DrawQuadCol_BottomLeft (const rmg::vec3& pos, const rmg::vec2& size, const Color& color)
    {
        RendererVertex v[4];
        vec4 col = color.ToPercent();
        v[0].SetPropCol (vec3(pos.x         , pos.y         , pos.z), col);
        v[1].SetPropCol (vec3(pos.x + size.x, pos.y         , pos.z), col);
        v[2].SetPropCol (vec3(pos.x + size.x, pos.y + size.y, pos.z), col);
        v[3].SetPropCol (vec3(pos.x         , pos.y + size.y, pos.z), col);
        Renderer::DrawGenericColor (v, RendererShapes::Shapes::Quad);
    }

    void DrawQuadTex (const rmg::vec3& pos, const rmg::vec2& size, const Color& color, unsigned int textureId)
    {
        RendererVertex v[4];
        rmg::vec2 halfSize = size / 2.0f;
        vec4 col = color.ToPercent();
        v[0].SetPropTex (vec3(pos.x - halfSize.x, pos.y - halfSize.y, pos.z), col, vec2(0.0f, 0.0f));
        v[1].SetPropTex (vec3(pos.x + halfSize.x, pos.y - halfSize.y, pos.z), col, vec2(1.0f, 0.0f));
        v[2].SetPropTex (vec3(pos.x + halfSize.x, pos.y + halfSize.y, pos.z), col, vec2(1.0f, 1.0f));
        v[3].SetPropTex (vec3(pos.x - halfSize.x, pos.y + halfSize.y, pos.z), col, vec2(0.0f, 1.0f));
        Renderer::DrawGeneric (v, RendererShapes::Shapes::Quad, textureId);
    }
    void DrawQuadTex_BottomLeft (const rmg::vec3& pos, const rmg::vec2& size, const Color& color, unsigned int textureId)
    {
        RendererVertex v[4];
        vec4 col = color.ToPercent();
        v[0].SetPropTex (vec3(pos.x         , pos.y         , pos.z), col, vec2(0.0f, 0.0f));
        v[1].SetPropTex (vec3(pos.x + size.x, pos.y         , pos.z), col, vec2(1.0f, 0.0f));
        v[2].SetPropTex (vec3(pos.x + size.x, pos.y + size.y, pos.z), col, vec2(1.0f, 1.0f));
        v[3].SetPropTex (vec3(pos.x         , pos.y + size.y, pos.z), col, vec2(0.0f, 1.0f));
        Renderer::DrawGeneric (v, RendererShapes::Shapes::Quad, textureId);
    }
}
}
