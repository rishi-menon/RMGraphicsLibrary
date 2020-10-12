#include "RMGraphics.h"
#include "Log/rmg_Log_int.h"
#include "Math/rmg_Random_int.h"

namespace RMGraphics {
    bool Initialise()
    {
        Log::Init ();
        Random::Init();
        
        return true;
    }

    void Cleanup()
    {

    }
}