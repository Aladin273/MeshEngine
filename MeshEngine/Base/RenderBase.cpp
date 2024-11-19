#include "RenderBase.h"

RenderBase::RenderBase()
{
    m_name = "RenderBase";
}

RenderBase::~RenderBase()
{

}

bool RenderBase::getRenderDataDirty() const
{
    return m_renderDataDirty;
}

bool RenderBase::getRenderSubDataDirty() const
{
    return m_renderSubDataDirty;
}

void RenderBase::setRenderDataDirty(bool dirty)
{
    m_renderDataDirty = dirty;
}

void RenderBase::setRenderSubDataDirty(bool dirty)
{
    m_renderSubDataDirty = dirty;
}

const std::vector<Vertex>& RenderBase::getRenderVertices() const
{
    return m_renderVertices;
}

const std::vector<uint32_t>& RenderBase::getRenderTriangles() const
{
    return m_renderTriangles;
}

const std::vector<uint32_t>& RenderBase::getRenderLines() const
{
    return m_renderLines;
}

const std::vector<uint32_t>& RenderBase::getRenderHoles() const
{
    return m_renderHoles;
}

const std::vector<uint32_t>& RenderBase::getRenderBoundaries() const
{
    return m_renderBoundaries;
}

const std::vector<uint32_t>& RenderBase::getRenderSubData() const
{
    return m_renderSubData;
}