#pragma once
#include "Renderer/rmg_RendererShapes_int.h"

namespace rmg {
    class RendererVertex;

namespace Renderer {
    bool Init();
    void Cleanup();

    void Flush();

    void OnWindowResize (int width, int height);

    unsigned int GetWhiteTexId ();

    void DrawGeneric(const RendererVertex* vertexBuffer, RendererShapes::Shapes shape, unsigned int ntexId);    
    void DrawGeneric (const RendererVertex* vertexBuffer, unsigned int nVertexCount, unsigned int* indexBuffer, unsigned int nIndexCount, unsigned int nTexId);

    void DrawGenericColor (const RendererVertex* vertexBuffer, unsigned int nVertexCount, unsigned int* indexBuffer, unsigned int nIndexCount);
    void DrawGenericColor(const RendererVertex* vertexBuffer, RendererShapes::Shapes shape);
}
}