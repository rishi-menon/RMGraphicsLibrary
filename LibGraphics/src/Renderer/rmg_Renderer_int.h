#pragma once

namespace rmg {
    class RendererVertex;

namespace Renderer {
    bool Init();
    void Cleanup();

    void Flush();

    void OnWindowResize (int width, int height);

    void DrawGeneric (const RendererVertex* vertexBuffer, unsigned int nVertexCount, unsigned int* indexBuffer, unsigned int nIndexCount, unsigned int nTexId);
}
}