
//This header file is multipurpose... This is the main header file which will be imported by the client in the end. But this header file is also used internally for building the library
#include "Log/rmg_Log.h"
#include "Math/rmg_Math.h"

namespace RMGraphics {
    bool Initialise();
    void Cleanup();
}