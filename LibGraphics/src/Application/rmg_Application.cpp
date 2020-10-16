#include "rmg_Application.h"
#include "rmg_Constants_int.h"
#include "rmg_Constants.h"
#include "GL/glew.h"
#include <GLFW/glfw3.h>

#include "Log/rmg_Log_int.h"
#include "Input/rmg_Input.h"
#include "Input/rmg_Input_int.h"
#include "Time/rmg_Time_int.h"
#include "Renderer/rmg_Renderer_int.h"
#include "Renderer/rmg_Renderer.h"

#include "Renderer/rmg_RendererVertex_int.h"

namespace rmg {
Application::Application() :
    m_pWindow(nullptr),
    m_userData(nullptr),
    m_WindowResizeFunc (nullptr),
    m_KeyCallbackFunc(nullptr),
    m_MousePosFunc(nullptr),
    m_MouseButtonFunc(nullptr)
{
    if (g_pCurrentApp)
    {
        RMG_LOG_IERROR ("RMG Error: An Application was already created. You cannot create multiple objects");
        IASSERT (false, "");
    }
}

bool Application::IsRunning() const
{
    return !glfwWindowShouldClose (m_pWindow);
}
//Gets called at the beginning of the frame
void Application::ClearScreen ()
{
    glClear (GL_COLOR_BUFFER_BIT);
    Time::CalculateDeltaTime();

    //Set mouse pos
    double x, y;
    glfwGetCursorPos (m_pWindow, &x, &y);
    Input::SetMousePos ((int)x, (int)y);
}
void Application::UpdateScreen ()
{
#if 0
    RendererVertex vertex[4];
    vertex[0].SetPropCol ({-0.5, -0.5, 0}, Color::red);
    vertex[1].SetPropCol ({0.5, -0.5, 0}, Color::red);
    vertex[2].SetPropCol ({0.5, 0.5, 0}, Color::blue);
    vertex[3].SetPropCol ({-0.5, 0.5, 0}, Color::blue);
#else
    RendererVertex vertex[4];
    vertex[0].SetPropCol ({50, 50, 0}, Color::red);
    vertex[1].SetPropCol ({400, 50, 0}, Color::red);
    vertex[2].SetPropCol ({400, 400, 0}, Color::blue);
    vertex[3].SetPropCol ({50, 400, 0}, Color::blue);
#endif
    unsigned int index[6] = {0, 1, 2, 2, 3, 0};

    Renderer::DrawGeneric (vertex, 4, index, 6, 0);


    Renderer::Flush();

    //Input::UpdateKeyStatus need to be called BEFORE polling for events. This is because keyDown gets detected only once we poll for events and we need to wait atleast for one frame before updating keys for it to have any effect on the client side
    Input::UpdateKeyStatus ();

    glfwSwapBuffers (m_pWindow);
    glfwPollEvents();
    // RMG_LOG_IWARN ("End");
}
void Application::SetUserData (void* data)
{
    m_userData = data;
}
void* Application::GetUserData() const
{
    return m_userData;
}
void Application::SetWindowResizeCallback(WindowResizeCallbackFunc func)
{
    m_WindowResizeFunc= func;
}
void Application::SetKeyCallback(KeyCallbackFunc func)
{
    m_KeyCallbackFunc = func;
}
void Application::SetMousePositionCallback(MousePositionCallbackFunc func)
{
    m_MousePosFunc = func;
}
void Application::SetMouseButtonCallback (MouseButtonCallbackFunc func)
{
   m_MouseButtonFunc = func;
}
    
bool Application::Initialise(const ivec2& size, const char* const strWindowTitle /* ="Application"*/)
{
    if (g_pCurrentApp)
    {
        return false;
    }
    g_pCurrentApp = this;
    RMG_LOG_ITRACE ("Created application: width {0}, height {1}", size.x, size.y);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    //Create the window
    m_pWindow = glfwCreateWindow (size.x, size.y, strWindowTitle, nullptr, nullptr);
    if (!m_pWindow)
    {
        IASSERT (false, "RMG Error: Could not create a GLFW window in OnInitialise. width {0}, height {1}, title {2}", size.x, size.y, strWindowTitle);
        return false;
    }
#if 0
    glDebugMessageCallback ([](GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam) {
                RMG_LOG_IERROR ("RMG OpenGL error: Type {0}, Id {1}, Severity {2}", type, id, severity);
                RMG_LOG_IERROR ("\t\t: {0}", message);
                IASSERT (false, "");
            }, nullptr);
#endif


    glfwMakeContextCurrent(m_pWindow);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        IASSERT (false, "RMG Error: Could not initialse GLEW");
        return false;
    }
    Input::SetScreenWidth (size.x);
    Input::SetScreenHeight (size.y);

    //Print OpenGl details
    {
        RMG_LOG_IINFO ("Vendor: {0}", glGetString(GL_VENDOR));
        RMG_LOG_IINFO ("Renderer: {0}", glGetString(GL_RENDERER));
        RMG_LOG_IINFO ("Version: {0}", glGetString(GL_VERSION));
        RMG_LOG_IINFO ("GLSL Version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));

        int nTotalTexSlots;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nTotalTexSlots);
        RMG_LOG_IINFO ("Total texture slots: {0}", nTotalTexSlots);
    }

    if (!Renderer::Init())
    {
        IASSERT (false, "RMG Error: Could not initialise Renderer");
        return false;
    }
    Renderer::OnWindowResize (size.x, size.y);

    //Setup callback
    glfwSetWindowUserPointer (m_pWindow, this);
    glfwSetWindowSizeCallback (m_pWindow, Input::WindowResizeCallback);
    glfwSetKeyCallback (m_pWindow, Input::KeyCallback);
    glfwSetMouseButtonCallback (m_pWindow, Input::MouseButtonCallback);
    glfwSetCursorPosCallback (m_pWindow, Input::MousePositionCallback);

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
