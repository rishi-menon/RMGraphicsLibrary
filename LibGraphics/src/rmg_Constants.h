#pragma once
#include "stdint.h"
namespace rmg {

    //This is for mac for the time being. Not tested for windows
    enum class Keycode : int32_t {
        //This NEEDS to be calculate manually at the moment. Also preferabbly do not change the order of the values.. If you do, consider looking at Input::Init() and checking if it would still work
        KeycodeCount = 80,

        A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Key0 = 48, Key1, Key2, Key3, Key4, Key5, Key6, Key7, Key8, Key9,
        Tilde = 96,
        Tab = 258,
        Capslock = 280,
        LeftShift = 340,
        FunctionKey = -1,
        LeftControl = 341,
        LeftOption = 342,
        LeftCommand = 343,
        Space = 32,
        RightCommand = 347,
        RightOption = 346,
        RightShift = 344,
        Enter = 257,
        Delete = 259,

        RightArrow = 262,
        LeftArrow = 263,
        DownArrow = 264,
        UpArrow = 265,

        Minus = 45,
        Plus = 61,
        Escape = 256,
        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        KeyComma = 44,
        KeyPeriod = 46,
        KeyForwardSlash = 47,
        KeySemicolon = 59,
        KeyQuote = 39,
        KeySquareBracketOpen = 91,
        KeySquareBracketClose = 93,
        KeyBackSlash = 92   //77 keys so far excluding KeycodeCount
    };
}