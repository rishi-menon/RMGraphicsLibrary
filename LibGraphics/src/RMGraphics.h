#pragma once
#include "Log/rmg_Log.h"
#include "Math/rmg_Math.h"
#include "Application/rmg_Application.h"
#include "Time/rmg_Time.h"
#include "Input/rmg_Input.h"
#include "rmg_Constants.h"
#include "Renderer/rmg_Renderer.h"


namespace rmg {
    bool Initialise();
    void Cleanup();

    void Sleep (uint32_t nMilliseconds);
}