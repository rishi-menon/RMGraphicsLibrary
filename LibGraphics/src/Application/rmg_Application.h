#pragma once
#include "../Math/rmg_Math.h"

class GLFWwindow;

namespace RMGraphics {
class Application 
{
public:
    //Constructors
    Application();
    Application(const Application&) = delete;
    Application& operator= (const Application&) = delete;
    ~Application() = default;

    bool IsRunning () const;
    void ClearScreen ();
    void UpdateScreen ();
    void SetUserData (void* data);
    void* GetUserData() const; 

    bool Initialise (const ivec2& size, const char* const strWindowTitle="Application");
    void Cleanup();
    
private:
    ivec2 m_size;
    GLFWwindow* m_pWindow;
    void* m_userData;
};

}