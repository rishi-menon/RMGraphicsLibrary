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

#include "Texture/rmg_Texture_int.h"
#include "Texture/rmg_Texture.h"

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

Application::~Application()
{
    if (m_pWindow)
    {
        Cleanup();
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
    //The x and y coordinates are obtained from glfw which takes top left of the screen as the origin. In our application the bottom left is the origin so we need to convert the y coordinate
    y = Input::GetScreenHeight() - y;
    Input::SetMousePos (x, y);
}
void Application::UpdateScreen ()
{
    #if 0
    RendererVertex vertex[4];
    vertex[0].SetPropTex ({10, 10, 0}, Color::white, {0.0f, 0.0f});
    vertex[1].SetPropTex ({200, 10, 0}, Color::white, {1.0f, 0.0f});
    vertex[2].SetPropTex ({200, 200, 0}, Color::white, {1.0f, 1.0f});
    vertex[3].SetPropTex ({10, 200, 0}, Color::white, {0.0f, 1.0f});

    RendererVertex vertex1[4];
    vertex1[0].SetPropTex ({300, 10, 0}, Color::pink, {0.0f, 0.0f});
    vertex1[1].SetPropTex ({400, 10, 0}, Color::white, {1.0f, 0.0f});
    vertex1[2].SetPropTex ({400, 200, 0}, Color::pink, {1.0f, 1.0f});
    vertex1[3].SetPropTex ({300, 200, 0}, Color::white, {0.0f, 1.0f});

    RendererVertex vertex2[4];
    vertex2[0].SetPropTex ({50, 500, 0}, Color::white, {0.0f, 0.0f});
    vertex2[1].SetPropTex ({200, 500, 0}, Color::white, {1.0f, 0.0f});
    vertex2[2].SetPropTex ({200, 700, 0}, Color::white, {1.0f, 1.0f});
    vertex2[3].SetPropTex ({50, 700, 0}, Color::white, {0.0f, 1.0f});

    RendererVertex vertex3[4];
    vertex3[0].SetPropTex ({500, 500, 0}, Color::pink, {0.0f, 0.0f});
    vertex3[1].SetPropTex ({600, 500, 0}, Color::white, {1.0f, 0.0f});
    vertex3[2].SetPropTex ({600, 700, 0}, Color::pink, {1.0f, 1.0f});
    vertex3[3].SetPropTex ({500, 700, 0}, Color::white, {0.0f, 1.0f});


    static unsigned int id = Texture::LoadTexture ("bg.png");
    unsigned int index[6] = {0, 1, 2, 2, 3, 0};

    Renderer::DrawGeneric (vertex, RendererShapes::Shapes::Quad, id);
    Renderer::DrawGenericColor (vertex1, RendererShapes::Shapes::Quad);
    Renderer::DrawGeneric (vertex2, RendererShapes::Shapes::Quad, id);
    Renderer::DrawGenericColor (vertex3, RendererShapes::Shapes::Quad);
#endif

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

    rmg_glcall(glEnable (GL_BLEND));
    rmg_glcall(glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

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
    if (!m_pWindow) { return; } //Most probably it has already been cleaned

	glfwDestroyWindow(m_pWindow);
	m_pWindow = nullptr;
	
}
}
