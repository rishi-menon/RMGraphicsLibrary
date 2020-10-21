#pragma once
#include <stdint.h>
#include "../rmg_Constants.h"

class GLFWwindow;
namespace rmg {
namespace Input {
    void SetScreenWidth(uint32_t n);
    void SetScreenHeight(uint32_t n);
    void SetMousePos(float x, float y);
    
    //Callback functions
    extern void WindowResizeCallback(GLFWwindow* pWindow, int nWidth, int nHeight);
    extern void MouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods);
    extern void MousePositionCallback(GLFWwindow* pWindow, double xpos, double ypos);
    extern void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods);

    int32_t GetIndexFromKeyCode (Keycode key);
    void UpdateKeyStatus();
    void Init ();
    
}
}