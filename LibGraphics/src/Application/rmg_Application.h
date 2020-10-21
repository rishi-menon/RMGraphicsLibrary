#pragma once
#include "../Math/rmg_Math.h"
#include "../rmg_Constants.h"

struct GLFWwindow;
namespace rmg {
namespace Input {
    extern void WindowResizeCallback(GLFWwindow* pWindow, int nWidth, int nHeight);
    extern void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    extern void MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    extern void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}
}

namespace rmg {
class Application 
{
    typedef void (*WindowResizeCallbackFunc)(Application* pWindow, int nWidth, int nHeight);
    typedef void (*KeyCallbackFunc)(Application* pWindow, Keycode key, int action, int mods);
    typedef void (*MousePositionCallbackFunc)(Application* pWindow, int x, int y);
    typedef void (*MouseButtonCallbackFunc)(Application* pWindow, int button, int action, int mods);
    //Friend functions for callback events
    friend void ::rmg::Input::WindowResizeCallback(GLFWwindow* pWindow, int nWidth, int nHeight);
    friend void ::rmg::Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    friend void ::rmg::Input::MousePositionCallback(GLFWwindow* window, double xpos, double ypos);
    friend void ::rmg::Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    //Constructors
    Application();
    Application(const Application&) = delete;
    Application& operator= (const Application&) = delete;
    ~Application();

    //Trivial functions
    bool IsRunning () const;
    void ClearScreen ();
    void UpdateScreen ();
    void SetUserData (void* data);
    void* GetUserData() const; 
    //Set Callback
    void SetWindowResizeCallback(WindowResizeCallbackFunc func);
    void SetKeyCallback(KeyCallbackFunc func);
    void SetMousePositionCallback(MousePositionCallbackFunc func);
    void SetMouseButtonCallback (MouseButtonCallbackFunc func);

    bool Initialise (const ivec2& size, const char* const strWindowTitle="Application");
    void Cleanup();

    
    
private:
    GLFWwindow* m_pWindow;
    void* m_userData;

    WindowResizeCallbackFunc m_WindowResizeFunc;
    KeyCallbackFunc m_KeyCallbackFunc;
    MousePositionCallbackFunc m_MousePosFunc;
    MouseButtonCallbackFunc m_MouseButtonFunc;
};

}