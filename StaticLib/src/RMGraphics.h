#pragma once
#include "Log/rmg_Log.h"
#include "Math/rmg_Math.h"
#include "Application/rmg_Application.h"
#include "Time/rmg_Time.h"
#include "Input/rmg_Input.h"
#include "rmg_Constants.h"
#include "Renderer/rmg_Renderer.h"
#include "Texture/rmg_Texture.h"
#include "Texture/rmg_Font.h"

namespace rmg {
    bool Initialise(int argc, const char* argv[]);
    void Cleanup();

    void Sleep (uint32_t nMilliseconds);
}