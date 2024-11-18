#include "PlaneNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEditor/Application/View.h"

PlaneNode::PlaneNode(View* view, glm::vec3 worldUp, float width, float height, uint32_t numSubs)
{
    renderTriangles = false;
    renderLines = true;

    m_mesh = Mesh::createPlane(worldUp, width, height, numSubs);
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "meshUnlitVertex.glsl", MeshEngine::Settings::shadersPath + "meshUnlitFragment.glsl");
    
    m_view = view;
}

PlaneNode::~PlaneNode()
{

}

void PlaneNode::start()
{
    if (m_view && m_mesh->getRenderDataDirty())
    {
        m_renderLinesId = m_view->getRenderSystem().bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderLines());
        m_mesh->setRenderDataDirty(false);
    }
}

void PlaneNode::end()
{
    if (m_view)
    {
        m_view->getRenderSystem().unbufferData(m_renderLinesId);
        m_mesh->setRenderDataDirty(true);
    }
}

void PlaneNode::update(float deltaTime)
{
    setRelativeTransform(glm::scale(glm::vec3(m_view->getViewport().getCamera().getDistanceToTarget())));
    super::update(deltaTime);
}

