#pragma once
namespace rmg {
namespace RendererShapes
{
    enum class Shapes
    {
        Quad = 0,

        Count
    };

    void Init();
    unsigned int* GetIndexBuffer(Shapes shape);
    unsigned int GetIndexCount(Shapes shape);
    unsigned int GetVertexCount(Shapes shape);
}

}