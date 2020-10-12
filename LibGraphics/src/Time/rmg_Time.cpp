#include "rmg_Time.h"
#include <GLFW/glfw3.h>

namespace RMGraphics {
namespace Time {
    double GetTime()
    {
        return glfwGetTime();
    }
}
}

