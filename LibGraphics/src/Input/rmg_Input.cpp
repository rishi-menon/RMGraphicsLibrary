#include "rmg_Input.h"
#include "rmg_Input_int.h"
#include "rmg_Constants.h"
#include "rmg_Constants_int.h"
#include <GLFW/glfw3.h>

#include "Application/rmg_Application.h"
#include "Log/rmg_Log_int.h"
#include <stdint.h>
#include <unordered_map>
#include <vector>

#include "Renderer/rmg_Renderer_int.h"
#include "Renderer/rmg_Renderer.h"

namespace rmg {
//Variables
namespace Input
{
    static uint32_t s_nScreenWidth = 0;
    static uint32_t s_nScreenHeight = 0;

    static std::unordered_map<Keycode, int32_t> s_mapKeycodeToIndex;

    static float s_fDeltaTime = 0;
    static float s_fLastTime = 0;
    
    static rmg::vec2 s_vMousePos = rmg::vec2(0.0f,0.0f);

    struct KeyStateTracker
    {
        //KeyDown: 10, KeyPress:11, KeyRelease:01
        bool bKeyDown;
        bool bKeyUp;
        
    };
    KeyStateTracker s_keysState[(int32_t)(Keycode::KeycodeCount)];
    std::vector<int32_t> s_keysToUpdateIndex;     //These are the index of the keys that were pressed/released in this frame, ie if it was just pressed and not released then we need to set it to isPressed so that KeyDown doesnt trigger
}
//Functions
namespace Input {
    

    uint32_t GetScreenWidth() { return s_nScreenWidth; }
    uint32_t GetScreenHeight() { return s_nScreenHeight; }
    void SetScreenWidth(uint32_t n) { s_nScreenWidth = n;}
    void SetScreenHeight(uint32_t n) { s_nScreenHeight = n;}
    
    rmg::vec2 GetMousePos() { return s_vMousePos; }
    void SetMousePos(float x, float y) { s_vMousePos.x = x; s_vMousePos.y = y; }

    ////////////////////////////////////////////////////////
    /////            Callback functions                 ////
    ////////////////////////////////////////////////////////
        
    void WindowResizeCallback(GLFWwindow* pWindow, int nWidth, int nHeight)
    {
        s_nScreenWidth = nWidth;
        s_nScreenHeight = nHeight;
        Renderer::OnWindowResize (nWidth, nHeight);

        Application* pApp = (Application*)glfwGetWindowUserPointer (pWindow);
        IASSERT (pApp, "RMG Error: Window user data was not set in callback function");
        if (pApp && pApp->m_WindowResizeFunc)
        {
            pApp->m_WindowResizeFunc(pApp, nWidth, nHeight);
        }

        int x, y;
        glfwGetWindowPos (pWindow, &x, &y);
        glfwSetWindowPos (pWindow, x+1, y+1);
        glfwSetWindowPos (pWindow, x, y);
        
    }
    void MouseButtonCallback(GLFWwindow* pWindow, int button, int action, int mods)
    {
        // RMG_LOG_IINFO ("mouse {0}, action {1}, mods {2}", button, action, mods);
        Application* pApp = (Application*)glfwGetWindowUserPointer (pWindow);
        IASSERT (pApp, "RMG Error: Window user data was not set in callback function");
        if (pApp && pApp->m_MouseButtonFunc)
        {
            pApp->m_MouseButtonFunc(pApp, button, action, mods);
        }
    }
    void MousePositionCallback(GLFWwindow* pWindow, double xpos, double ypos)
    {
        Application* pApp = (Application*)glfwGetWindowUserPointer (pWindow);
        IASSERT (pApp, "RMG Error: Window user data was not set in callback function");

        ypos = s_nScreenHeight - ypos;
        if (pApp && pApp->m_MousePosFunc)
        {
            pApp->m_MousePosFunc(pApp, xpos, ypos);
        }
    }
    void KeyCallback(GLFWwindow* pWindow, int key, int scancode, int action, int mods)
    {
        // RMG_LOG_IINFO ("key {0}, scancode {1}, action {2}, mods {3}", key, scancode, action, mods);
        Application* pApp = (Application*)glfwGetWindowUserPointer (pWindow);
        IASSERT (pApp, "RMG Error: Window user data was not set in callback function");

        int32_t keyIndex = GetIndexFromKeyCode((Keycode)(key));
        KeyStateTracker& keyState = s_keysState[keyIndex];
        s_keysToUpdateIndex.emplace_back (keyIndex);
        keyState.bKeyDown = (bool)(action); //Gets set to true when the key was just pressed... If you think about it as a two bit bit-shifter then bKeyUp will be equal to bKeyUp in equlibrium state.
        

        if (pApp && pApp->m_KeyCallbackFunc)
        {
            pApp->m_KeyCallbackFunc(pApp, (Keycode)(key), action, mods);
        }
    }

    
    ////////////////////////////////////////////////////////
    //////                  Init                      //////
    ////////////////////////////////////////////////////////

    int32_t GetIndexFromKeyCode (Keycode key)
    {
        std::unordered_map<Keycode, int32_t>::const_iterator it = s_mapKeycodeToIndex.find (key);
        if (it == s_mapKeycodeToIndex.end())
        {
            RMG_LOG_IWARN ("RMG Warning: keycode {0} is not registered in the index map...", (int32_t)(key));
            return 0;
        }
        return it->second;
    }

    bool GetKeyDown (Keycode key)
    {
        int32_t index = GetIndexFromKeyCode (key);
        const KeyStateTracker& keyTracker = s_keysState[index];
        return keyTracker.bKeyDown && !keyTracker.bKeyUp;
    } 
    bool GetKeyUp (Keycode key)
    {
        int32_t index = GetIndexFromKeyCode (key);
        const KeyStateTracker& keyTracker = s_keysState[index];
        return !keyTracker.bKeyDown && keyTracker.bKeyUp;
    }
    bool GetKey (Keycode key)
    {
        int32_t index = GetIndexFromKeyCode (key);
        const KeyStateTracker& keyTracker = s_keysState[index];
        return keyTracker.bKeyDown || keyTracker.bKeyUp;
    }

    void UpdateKeyStatus()
    {
        uint32_t size = s_keysToUpdateIndex.size();
        if (!size) return;

        for (uint32_t i = 0; i < size; i++)
        {
            KeyStateTracker& keyState = s_keysState[s_keysToUpdateIndex[i]];
            
            //It acts like a two bit shift register essentially... Store 1 when key is pressed.. This way you can check if a key was pressed or released recently
            keyState.bKeyUp = keyState.bKeyDown;
        }
        s_keysToUpdateIndex.clear();
        
    }

    void Init()
    {
        s_fLastTime = 0;
        s_fDeltaTime = 0;
        // std::memset (s_keysState, 0, sizeof(KeyStateTracker)*(int)(Keycode::KeycodeCount));
        for (int i = 0; i < (int)(Keycode::KeycodeCount); i++)
        {
            s_keysState[i] = { false, false};
        }
        s_keysToUpdateIndex.reserve (10);

        //Calculate index map
        s_mapKeycodeToIndex.reserve ((int32_t)(Keycode::KeycodeCount));
        int32_t index = 0;
        //Set index for 26 letters
        for (int i = 0; i < 26; i++)
        {
            Keycode key = (Keycode)((int)Keycode::A + i);
            s_mapKeycodeToIndex.emplace (key, index);
            index++;
        }
        //Set index for 0-9 num
        for (int i = 0; i < 10; i++)
        {
            Keycode key = (Keycode)((int)Keycode::Key0 + i);
            s_mapKeycodeToIndex.emplace (key, index);
            index++;
        }
        //Set index for the function (F1 - F12) keys
        for (int i = 0; i < 12; i++)
        {
            Keycode key = (Keycode)((int)Keycode::F1 + i);
            s_mapKeycodeToIndex.emplace (key, index);
            index++;
        }

        //Misc
        s_mapKeycodeToIndex.emplace (Keycode::Tilde, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Tab, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Capslock, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Space, index++);
#if 0
        s_mapKeycodeToIndex.emplace (Keycode::LeftShift, index++);
        s_mapKeycodeToIndex.emplace (Keycode::FunctionKey, index++);
        s_mapKeycodeToIndex.emplace (Keycode::LeftControl, index++);
        s_mapKeycodeToIndex.emplace (Keycode::LeftOption, index++);
        s_mapKeycodeToIndex.emplace (Keycode::LeftCommand, index++);
        s_mapKeycodeToIndex.emplace (Keycode::RightCommand, index++);
        s_mapKeycodeToIndex.emplace (Keycode::RightOption, index++);
        s_mapKeycodeToIndex.emplace (Keycode::RightShift, index++);
#endif
        s_mapKeycodeToIndex.emplace (Keycode::Enter, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Delete, index++);
        s_mapKeycodeToIndex.emplace (Keycode::RightArrow, index++);
        s_mapKeycodeToIndex.emplace (Keycode::LeftArrow, index++);
        s_mapKeycodeToIndex.emplace (Keycode::DownArrow, index++);
        s_mapKeycodeToIndex.emplace (Keycode::UpArrow, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Minus, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Plus, index++);
        s_mapKeycodeToIndex.emplace (Keycode::Escape, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeyComma, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeyPeriod, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeyForwardSlash, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeySemicolon, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeyQuote, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeySquareBracketOpen, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeySquareBracketClose, index++);
        s_mapKeycodeToIndex.emplace (Keycode::KeyBackSlash, index++);
    }
}
}