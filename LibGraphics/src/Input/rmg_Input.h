#pragma once
#include "../rmg_Constants.h"
#include <stdint.h>
#include "Math/rmg_Math.h"

namespace rmg {
namespace Input {
    uint32_t GetScreenWidth();
    uint32_t GetScreenHeight();

    bool GetKeyDown (Keycode key); 
    bool GetKeyUp (Keycode key);
    bool GetKey (Keycode key);

    ivec2 GetMousePos();

}
}