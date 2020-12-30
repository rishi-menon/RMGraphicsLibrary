#include "rmg_Constants.h"
#include "rmg_Constants_int.h"
#include "rmg_RendererShapes_int.h"

namespace rmg {
namespace RendererShapes
{
    
    static unsigned int* s_arrIndexBuffers[(int)(Shapes::Count)];
    static unsigned int s_arrIndexCount[(int)(Shapes::Count)];
    static unsigned int s_arrVertexCount[(int)(Shapes::Count)];
    //Quad
    static unsigned int s_indexQuad[6] = {0, 1, 2, 2, 3, 0};

    void Init()
    {
        int i = (int)(Shapes::Quad);
        s_arrIndexBuffers[i] = s_indexQuad;
        s_arrIndexCount[i] = 6;
        s_arrVertexCount[i] = 4;
        
         
    }

    unsigned int* GetIndexBuffer(Shapes shape)
    {
        return s_arrIndexBuffers[(int)(shape)];
    }
    unsigned int GetIndexCount(Shapes shape)
    {
        return s_arrIndexCount[(int)(shape)];
    }
    unsigned int GetVertexCount(Shapes shape)
    {
        return s_arrVertexCount[(int)(shape)];
    }


}

}