#pragma once

#include "MeshEngine/Base/Base.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"

class RenderBase : public Base
{
public:
    RenderBase();
    virtual ~RenderBase();

public:
    bool getRenderDataDirty() const;
    bool getRenderSubDataDirty() const;

    void setRenderDataDirty(bool dirty);
    void setRenderSubDataDirty(bool dirty);

public:
    const std::vector<Vertex>& getRenderVertices() const;
    
    const std::vector<uint32_t>& getRenderTriangles() const;
    const std::vector<uint32_t>& getRenderLines() const;
    const std::vector<uint32_t>& getRenderHoles() const;
    const std::vector<uint32_t>& getRenderBoundaries() const;
    
    const std::vector<uint32_t>& getRenderSubData() const;

protected:
    std::vector<Vertex> m_renderVertices;
    std::vector<uint32_t> m_renderTriangles;
    std::vector<uint32_t> m_renderLines;
    std::vector<uint32_t> m_renderHoles;
    std::vector<uint32_t> m_renderBoundaries;
    std::vector<uint32_t> m_renderSubData;

protected:
    bool m_renderDataDirty = true;
    bool m_renderSubDataDirty = true;
};