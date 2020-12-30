#include "RMGraphics.h"
#include "Log/rmg_Log_int.h"
#include "Math/rmg_Random_int.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include "Input/rmg_Input.h"
#include "Input/rmg_Input_int.h"
#include "Renderer/rmg_Renderer_int.h"
#include "Renderer/rmg_Renderer.h"
#include "rmg_Constants.h"
#include "rmg_Constants_int.h"
#include "Texture/rmg_Texture_int.h"
#include "Texture/rmg_Texture.h"
#include "Renderer/rmg_RendererShapes_int.h"
#include "Texture/rmg_Font.h"

namespace rmg {
    bool Initialise(int argc, const char* argv[])
    {
        if (g_Argc != -1)
        {
            RMG_LOG_IERROR ("RMG Error: rmg has already been initialised");
            return false;
        }
        if (argc >= g_nMaxArgc)
        {
            RMG_LOG_IERROR ("RMG Error: rmg can only support {0} arguments: {1} arguments given", g_nMaxArgc, argc);
            return false;
        }
        g_Argc = argc;
        for (int i = 0; i < argc; i++)
        {
            g_Argv[i] = argv[i];
        }

        Log::Init();
        Random::Init();
        Input::Init();
        Texture::Init();
        Font::OnLibInit();
        RendererShapes::Init();
        
        glfwSetErrorCallback([](int error, const char* const desc)
	    {
		    IASSERT(false, "RMG GLFW Error {0}: {1}", error, desc);
	    });

        
        if (!glfwInit())
	    {
		    IASSERT(false, "Could not initialise GLFW");
		    return false;
	    }
        return true;
    }

    void Cleanup()
    {
        /*Texture::Cleanup();
        Font::OnLibCleanup();
        Renderer::Cleanup();*/
        
        glfwTerminate();
    }

    void Sleep (uint32_t nMilliseconds)
    {
        std::chrono::milliseconds sleepTime(nMilliseconds);
        std::this_thread::sleep_for (sleepTime);
    }
}