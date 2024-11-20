#include "LightNode.h"

#include "MeshEngine/Scene/Scene.h"

LightNode::LightNode()
{
    m_name = "LightNode";
    m_visible = MeshEngine::g_editor;
    m_renderMesh = std::make_unique<Mesh>(HalfEdgeTable<Vertex>());
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseVertex.glsl", MeshEngine::Settings::shadersPath + "baseFragment.glsl");
}

LightNode::~LightNode()
{

}

const BoundingBox& LightNode::getBoundingBox() const
{
    return m_renderMesh->getBoundingBox();
}

void LightNode::start()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_renderMesh->getRenderDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();
            m_renderId = renderSystem->bufferData(m_renderMesh->getRenderVertices(), m_renderMesh->getRenderTriangles());
            
            m_renderMesh->setRenderDataDirty(false);
        }
    }

    super::start();
}

void LightNode::end()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        RenderSystem* renderSystem = getScene()->getRenderSystem();
        renderSystem->unbufferData(m_renderId);

        m_renderMesh->setRenderDataDirty(true);
    }

    super::end();
}

void LightNode::update(float deltaTime)
{
    super::update(deltaTime);
}

void LightNode::render(RenderSystem* renderSystem)
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
