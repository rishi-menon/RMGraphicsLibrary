#include "RMGraphics.h"
#include "Log/rmg_Log_int.h"
#include "Math/rmg_Random_int.h"
#include <GLFW/glfw3.h>

namespace RMGraphics {
    bool Initialise()
    {
        Log::Init();
        Random::Init();

        glfwSetErrorCallback([](int error, const char* const desc)
	    {
		    IASSERT(false, "GLFW Error {0}: {1}", error, desc);
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
        glfwTerminate();
    }
}