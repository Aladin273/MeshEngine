#include "PlaneNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Base/Mesh.h"
#include "MeshEditor/Editor/View.h"


PlaneNode::PlaneNode(View* view, glm::vec3 worldUp, float width, float height, uint32_t numSubs)
{
    m_mesh = Mesh::createPlane(worldUp, width, height, numSubs);
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseColorVertex.glsl", MeshEngine::Settings::shadersPath + "baseColorFragment.glsl");

    m_view = view;
}

PlaneNode::~PlaneNode()
{

}

void PlaneNode::start()
{
    if (m_view && m_mesh->getRenderDataDirty())
    {
        m_renderId = m_view->getRenderSystem().bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderLines());
        m_mesh->setRenderDataDirty(false);
    }

    super::start();
}

void PlaneNode::end()
{
    if (m_view)
    {
        m_view->getRenderSystem().unbufferData(m_renderId);
        m_mesh->setRenderDataDirty(true);
    }

    super::end();
}

void PlaneNode::update(float deltaTime)
{
    setRelativeTransform(glm::scale(glm::vec3(m_view->getViewport().getCamera().getDistanceToTarget())));
    super::update(deltaTime);
}

void PlaneNode::render(RenderSystem* renderSystem)
{
    m_shader->bind();
    
    m_shader->setMat4("model", getAbsoluteTransform());
    m_shader->setVec4("color", glm::vec4(0.5f, 0.5f, 0.5f, 1.f));

    renderSystem->bindData(m_renderId);

    renderSystem->setLineSize(1.0f);
    renderSystem->renderLines();

    renderSystem->unbindData();

    m_shader->unbind();

    super::render(renderSystem);
}

