#include "ArrowNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEditor/Editor/View.h"

ArrowNode::ArrowNode(View* view)
{
    m_mesh = Mesh::createArrow(MeshEngine::Settings::axisX, 0.06f, 0.1f, 0.02f, 0.8f, 32);
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "meshUnlitVertex.glsl", MeshEngine::Settings::shadersPath + "meshUnlitFragment.glsl");
    
    m_view = view;
}

ArrowNode::~ArrowNode()
{

}

void ArrowNode::start()
{
    if (m_view && m_mesh->getRenderDataDirty())
    {
        m_renderTrianglesId = m_view->getRenderSystem().bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderTriangles());
        m_mesh->setRenderDataDirty(false);
    }
}

void ArrowNode::end()
{
    if (m_view)
    {
        m_view->getRenderSystem().unbufferData(m_renderTrianglesId);
    }
}

void ArrowNode::update(float deltaTime)
{
    super::update(deltaTime);
}

