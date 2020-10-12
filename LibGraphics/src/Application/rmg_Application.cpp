#include "rmg_Application.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "Log/rmg_Log_int.h"

namespace RMGraphics {

static ::RMGraphics::Application* s_pApp = nullptr;
Application::Application() :
    m_size (0,0),
    m_pWindow(nullptr),
    m_userData(nullptr)
{
    if (s_pApp)
    {
        RMG_LOG_IERROR ("RMG Error: An Application was already created. You cannot create multiple objects");
        IASSERT (false, "");
    }
}

bool Application::IsRunning() const
{
    return !glfwWindowShouldClose (m_pWindow);
}
void Application::ClearScreen ()
{
    glClear (GL_COLOR_BUFFER_BIT);
}
void Application::UpdateScreen ()
{
    glfwSwapBuffers (m_pWindow);
    glfwPollEvents();
}
void Application::SetUserData (void* data)
{
    m_userData = data;
}
void* Application::GetUserData() const
{
    return m_userData;
}
    
bool Application::Initialise(const ivec2& size, const char* const strWindowTitle /* ="Application"*/)
{
    if (s_pApp)
    {
        return false;
    }
    s_pApp = this;
    RMG_LOG_ITRACE ("Created application: width {0}, height {1}", size.x, size.y);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_pWindow = glfwCreateWindow (size.x, size.y, strWindowTitle, nullptr, nullptr);
    if (!m_pWindow)
    {
        RMG_LOG_IERROR ("RMG Error: Could not create a GLFW window in OnInitialise. width {0}, height {1}, title {2}", size.x, size.y, strWindowTitle);
        IASSERT (false, "");
        return false;
    }

    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        IASSERT (false, "RMG Error: Could not initialse GLEW");
    }

    return true;
}

void Application::Cleanup ()
{
    if (m_pWindow)
	{
		glfwDestroyWindow(m_pWindow);
		m_pWindow = nullptr;
	}
}
}
