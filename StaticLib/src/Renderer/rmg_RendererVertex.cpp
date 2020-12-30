#include "Renderer/rmg_RendererVertex_int.h"
#include "Math/rmg_Math.h"

namespace rmg {
    RendererVertex::RendererVertex(const rmg::vec3& pos, const rmg::vec4& col) :
        m_fTexId(0.0f)
    {
        SetPropCol (pos, col);
    }
    RendererVertex::RendererVertex(const rmg::vec3& pos, const rmg::vec4& col, const rmg::vec2& texCoords) :
        m_fTexId(0.0f)
    {
        SetPropTex (pos, col, texCoords);
    }
    RendererVertex::RendererVertex(const rmg::vec3& pos, const rmg::Color& col) :
        m_fTexId(0.0f)
    {
        SetPropCol (pos, col.ToPercent());
    }
    RendererVertex::RendererVertex(const rmg::vec3& pos, const rmg::Color& col, const rmg::vec2& texCoords) :
        m_fTexId(0.0f)
    {
        SetPropTex (pos, col.ToPercent(), texCoords);
    }

    void RendererVertex::SetPropCol (const rmg::vec3& pos, const rmg::vec4& col)
    {
        m_vPos = pos;
        m_vCol = col;
        m_vTexCoords = rmg::vec2 (0.0f, 0.0f);
    }
    void RendererVertex::SetPropTex (const rmg::vec3& pos, const rmg::vec4& col, const rmg::vec2& texCoords)
    {
        m_vPos = pos;
        m_vCol = col;
        m_vTexCoords = texCoords;
    }

    void RendererVertex::SetPropCol (const rmg::vec3& pos, const rmg::Color& col)
    {
        SetPropCol (pos, col.ToPercent());
    }

    void RendererVertex::SetPropTex (const rmg::vec3& pos, const rmg::Color& col, const rmg::vec2& texCoords)
    {
        SetPropTex (pos, col.ToPercent(), texCoords);
    }

    
}