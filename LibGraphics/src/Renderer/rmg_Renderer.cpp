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

static const char* s_strShaderFragment = R"(
#version 330 core

layout(location = 0) out vec4 col;

in vec4 v_col;
in vec2 v_texCoord;
in float v_texId;
uniform sampler2D u_textureSlots[16];

void main()
{
    int texId = int(v_texId);
    //col = v_col * texture(u_textureSlots[texId], v_texCoord);
    col = v_col;
}
)";


//Constants
static const unsigned int s_nMaxVertex = 12; 
static const unsigned int s_nMaxIndex = 18;
static const uint8_t s_nMaxTextureSlots = 16;

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

namespace rmg {
namespace Renderer {

    bool Init ()
    {
        if (s_vertexArray || s_indexArray)
        {
            RMG_LOG_IWARN ("RMG Warning: Renderer has already been initialised");
            return false;
        }
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
        s_nShader = glCreateProgram ();
        unsigned int nVertex = CompileShader(GL_VERTEX_SHADER, s_strShaderVertex);
        unsigned int nFragment = CompileShader(GL_FRAGMENT_SHADER, s_strShaderFragment);
        if (nVertex == (unsigned int)(-1) || nFragment == (unsigned int)(-1)) 
        {   
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
            return false;
        }
        rmg_glcall(glUseProgram (s_nShader));

        //Set uniforms
        int nTexId = glGetUniformLocation (s_nShader, "u_textureSlots");
        if (nTexId == -1)
        {
            RMG_LOG_IWARN ("RMG Warning: Uniform u_textureSlots was not found");
            // return false;
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

        s_vertexArray = new(std::nothrow) RendererVertex[s_nMaxVertex];
        s_indexArray = new(std::nothrow) unsigned int[s_nMaxIndex];
        s_boundTexSlots = new(std::nothrow) unsigned int[s_nMaxTextureSlots];

        if (!s_vertexArray || !s_indexArray || !s_boundTexSlots)
        {
            RMG_LOG_IERROR ("RMG Error: Could not allocate enough heap memory for the renderer");
            return false;
        }

        for (unsigned int i = 0; i < s_nMaxTextureSlots; i++)
        {
            s_boundTexSlots[i] = (unsigned int)(-1);    //Reset to an invalid state
        }
        return true;
    }

    void Flush ()
    {
        if (!s_nCurVertexCount || !s_nCurIndexCount)
        {
            return;
        }

        rmg_glcall(glBufferSubData (GL_ARRAY_BUFFER, 0, sizeof(RendererVertex) * s_nCurVertexCount, s_vertexArray));

        rmg_glcall(glBufferSubData (GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned int) * s_nCurIndexCount, s_indexArray));

        rmg_glcall(glDrawElements (GL_TRIANGLES, s_nCurIndexCount, GL_UNSIGNED_INT, 0));

        s_nCurIndexCount = 0;
        s_nCurVertexCount = 0;
        s_nCurTexIndex = 1; //0 is reserved for white 
        std::memset (s_boundTexSlots, -1, s_nMaxTextureSlots);  //reset the tex slots
        //To do: set 0 to white tex slot here
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

            // rmg_glcall(glActiveTexture (GL_TEXTURE0 + s_nCurTexIndex));
            // rmg_glcall(glBindTexture (GL_TEXTURE_2D, nTexId));
            s_boundTexSlots[s_nCurTexIndex] = nTexId;
            nTextureSlot = s_nCurTexIndex;
            ++s_nCurTexIndex;
        }

        //We have covered the safety checks where we dont have enough room in the current batch

        RendererVertex* vertexOffset = s_vertexArray + s_nCurVertexCount;
        // std::memcpy (vertexOffset, vertexBuffer, nVertexCount); //Perform a shallow copy

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

}
}
