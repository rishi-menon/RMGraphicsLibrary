#include "RMGraphics.h"
#include "Log/rmg_Log_int.h"
#include "Math/rmg_Random_int.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include "Input/rmg_Input.h"
#include "Input/rmg_Input_int.h"
#include "Renderer/rmg_Renderer_int.h"
#include "Renderer/rmg_Renderer.h"

namespace rmg {
    bool Initialise()
    {
        Log::Init();
        Random::Init();
        Input::Init();
        
        RMG_LOG_IWARN ("Val {0}", Input::GetIndexFromKeyCode(Keycode::C));
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
        Renderer::Cleanup();
        glfwTerminate();
    }

    void Sleep (uint32_t nMilliseconds)
    {
        std::chrono::milliseconds sleepTime(nMilliseconds);
        std::this_thread::sleep_for (sleepTime);
    }
}