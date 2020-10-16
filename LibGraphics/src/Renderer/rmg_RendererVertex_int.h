#include "Math/rmg_Math.h"
namespace rmg {
//Declarations copied from rmg_Renderer_int.h
namespace Renderer {
    bool Init();
    void Cleanup();
}

class RendererVertex
{
public:
    RendererVertex() = default;
    RendererVertex(const RendererVertex&) = default;
    RendererVertex(const rmg::vec3& pos, const rmg::vec4& col);
    RendererVertex(const rmg::vec3& pos, const rmg::vec4& col, const rmg::vec2& texCoords);
    RendererVertex(const rmg::vec3& pos, const rmg::Color& col);
    RendererVertex(const rmg::vec3& pos, const rmg::Color& col, const rmg::vec2& texCoords);

    void SetPropCol (const rmg::vec3& pos, const rmg::vec4& col);
    void SetPropCol (const rmg::vec3& pos, const rmg::Color& col);
    void SetPropTex (const rmg::vec3& pos, const rmg::vec4& col, const rmg::vec2& texCoords);
    void SetPropTex (const rmg::vec3& pos, const rmg::Color& col, const rmg::vec2& texCoords);

    void SetTexId (float id) { m_fTexId = id; }
    float GetTexId () { return m_fTexId; }

friend bool Renderer::Init();
friend void Renderer::Cleanup();

public:
    rmg::vec3 m_vPos;
    rmg::vec4 m_vCol;
    rmg::vec2 m_vTexCoords;
private:
    float m_fTexId;
};

}

