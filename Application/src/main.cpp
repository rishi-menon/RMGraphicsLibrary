#include <spdlog/spdlog.h>
#include <RMGraphics/RMGraphics.h>
int main()
{
    RMGraphics::Initialise();
    RMG_LOG_INFO ("Start");
    
    RMGraphics::vec2 vec (10, 11.54);
    // RMG_LOG_WARN ("{0}, {1}", vec.x, vec.y);


    RMGraphics::Cleanup();
    return 0;
}