#include "LightNode.h"

#include "MeshEngine/Model/Scene.h"

LightNode::LightNode()
{
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "meshUnlitVertex.glsl", MeshEngine::Settings::shadersPath + "meshUnlitFragment.glsl");
}

LightNode::~LightNode()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        RenderSystem* renderSystem = getScene()->getRenderSystem();
        renderSystem->unbufferData(m_renderTrianglesId);
    }
}

const BoundingBox& LightNode::getBoundingBox() const
{
    return m_mesh->getBoundingBox();
}

void LightNode::start()
{
    super::start();
}

void LightNode::end()
{
    super::end();
}

void LightNode::update(float deltaTime)
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_mesh->getRenderDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();
            renderSystem->unbufferData(m_renderTrianglesId);

            m_renderTrianglesId = renderSystem->bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderTriangles());
            m_mesh->setRenderDataDirty(false);
        }
    }

    super::update(deltaTime);
}

void LightNode::render(RenderSystem* renderSystem)
{
    m_shader->bind();
    m_shader->setMat4("model", getAbsoluteTransform());

    m_shader->setVec3("material.ambient", glm::vec3(0.f));
    m_shader->setVec3("material.diffuse", glm::vec3(0.f));
    m_shader->setVec3("material.specular", glm::vec3(0.f));
    m_shader->setVec3("material.emission", m_renderColor);
    m_shader->setFloat("material.shininess", 0);

    renderSystem->bindData(m_renderTrianglesId);
    renderSystem->renderTriangles();
    renderSystem->unbindData();

    m_shader->unbind();

    super::render(renderSystem);
}
