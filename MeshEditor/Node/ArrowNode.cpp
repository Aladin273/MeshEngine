#include "ArrowNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Base/Mesh.h"
#include "MeshEditor/Editor/View.h"

ArrowNode::ArrowNode(View* view, glm::vec3 axis, glm::vec3 color)
{
    m_mesh = Mesh::createArrow(axis, 0.06f, 0.1f, 0.02f, 0.8f, 32);
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseVertex.glsl", MeshEngine::Settings::shadersPath + "baseFragment.glsl");

    m_renderColor = color;
    m_view = view;
}

ArrowNode::~ArrowNode()
{

}

void ArrowNode::start()
{
    if (m_view && m_mesh->getRenderDataDirty())
    {
        m_renderId = m_view->getRenderSystem().bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderTriangles());
        m_mesh->setRenderDataDirty(false);
    }

    super::start();
}

void ArrowNode::end()
{
    if (m_view)
    {
        m_view->getRenderSystem().unbufferData(m_renderId);
    }

    super::end();
}

void ArrowNode::update(float deltaTime)
{
    super::update(deltaTime);
}

void ArrowNode::render(RenderSystem* renderSystem)
{
    m_shader->bind();

    m_shader->setMat4("model", getAbsoluteTransform());
    m_shader->setVec4("color", glm::vec4(m_renderColor, 1.f));

    renderSystem->bindData(m_renderId);

    renderSystem->renderTriangles();

    renderSystem->unbindData();

    m_shader->unbind();

    super::render(renderSystem);
}

