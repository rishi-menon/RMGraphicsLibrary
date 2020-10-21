#include "rmg_Constants.h"
#include "rmg_Constants_int.h"
#include "Texture/rmg_Texture.h"
#include "Texture/rmg_Texture_int.h"
#include "Log/rmg_Log_int.h"
#include "Log/rmg_Log.h"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <string>
#include <list>

namespace rmg
{
namespace Texture {
    static std::string s_strAppDir;
    static std::list<unsigned int> s_listTextureIds;

    const std::string& GetCWD() { return s_strAppDir; }

    bool Init()
    {
        const char* strAppPath = g_Argv[0];
        int nStrLen=0;
        for (; strAppPath[nStrLen] != '\0'; ++nStrLen);

        int index = nStrLen-1;
        for (; index >= 0; index--)
        {
            if (strAppPath[index] == '/')
            {
                break;
            }
        }
        if (index < 0)
        {
            IASSERT (false, "RMG Error: Could not calculate base directory in texture init");
            return false;
        }
        //copy till index
        s_strAppDir.reserve (index+2);
        for (int i = 0; i <= index; i++)
        {
            s_strAppDir.push_back (strAppPath[i]);
        }
        // RMG_LOG_ITRACE ("RMG Trace: Base directory {0}", s_strAppDir.c_str());
        return true;
    }
    unsigned int LoadTextureBuffer (unsigned char buffer[], int nSizeX, int nSizeY)
    {
        unsigned int nId;
        rmg_glcall(glGenTextures (1, &nId));
        glActiveTexture (GL_TEXTURE0);
        rmg_glcall(glBindTexture (GL_TEXTURE_2D, nId));
        rmg_glcall(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        rmg_glcall(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        rmg_glcall(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        rmg_glcall(glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        rmg_glcall(glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, nSizeX, nSizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer));
        
        s_listTextureIds.push_back (nId);

        return nId;
    }

    unsigned int LoadTexture (const char* strImagePath)
    {
        std::string strFullPath = s_strAppDir + strImagePath;
        stbi_set_flip_vertically_on_load(1);
        int width, height, bpp;
        unsigned char* buffer = stbi_load (strFullPath.c_str(), &width, &height, &bpp, 4);
        if (buffer)
        {
            unsigned int texId = Texture::LoadTextureBuffer (buffer, width, height);
            stbi_image_free(buffer);
            return texId;
        }
        else
        {
            RMG_LOG_IERROR ("RMG Error: Could not load texture: {0}", strImagePath);
            IASSERT (false, "");
            return (unsigned int)(-1);
        }
    }

    void Cleanup()
    {
        for (unsigned int id : s_listTextureIds)
        {
            rmg_glcall(glDeleteTextures (1, &id));
        }
    }
}
}
