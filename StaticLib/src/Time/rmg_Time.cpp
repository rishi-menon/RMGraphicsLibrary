#include "rmg_Time.h"
#include <GLFW/glfw3.h>
#include "Math/rmg_Math.h"

namespace rmg {
namespace Time {
    static float s_fDeltaTime = 0;
    static float s_fLastGameTime = 0;

    double GetTime()
    {
        return glfwGetTime();
    }

    void CalculateDeltaTime ()
    {
        float curTime = (float)Time::GetTime();
        s_fDeltaTime = Math::ClampRight (0.1f,  curTime - s_fLastGameTime);
        s_fLastGameTime = curTime;
    }
    float GetDeltaTime() { return s_fDeltaTime; }
}
}

