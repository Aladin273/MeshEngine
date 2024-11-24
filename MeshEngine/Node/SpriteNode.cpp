#include "SpriteNode.h"

#include "MeshEngine/Scene/Scene.h"

SpriteNode::SpriteNode()
{
    m_name = "SpriteNode";
    m_sprite = std::make_unique<Sprite>();
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseLitVertex.glsl", MeshEngine::Settings::shadersPath + "baseLitFragment.glsl");
}

SpriteNode::~SpriteNode()
{

}

const BoundingBox& SpriteNode::getBoundingBox() const
{
    return m_sprite->getBoundingBox();
}

void SpriteNode::start()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_sprite->getRenderDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();

            Material material = m_sprite->getMaterial();
            material.diffuseMap.id = renderSystem->bufferTexture(material.diffuseMap.path, material.diffuseMap.width, material.diffuseMap.height, true);
            material.specularMap.id = renderSystem->bufferTexture(material.specularMap.path, material.specularMap.width, material.specularMap.height, true);
            material.emissionMap.id = renderSystem->bufferTexture(material.emissionMap.path, material.emissionMap.width, material.emissionMap.height, true);
            m_sprite->setMaterial(material);

            m_renderTrianglesId = renderSystem->bufferData(m_sprite->getRenderVertices(), m_sprite->getRenderTriangles());

            m_sprite->setRenderDataDirty(false);
        }
    }

    super::start();
}

void SpriteNode::end()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        RenderSystem* renderSystem = getScene()->getRenderSystem();

        renderSystem->unbufferData(m_renderTrianglesId);

        renderSystem->unbufferTexture(m_sprite->getMaterial().diffuseMap.id);
        renderSystem->unbufferTexture(m_sprite->getMaterial().specularMap.id);
        renderSystem->unbufferTexture(m_sprite->getMaterial().emissionMap.id);

        m_sprite->setRenderDataDirty(true);
    }

    super::end();
}

void SpriteNode::update(float deltaTime)
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_sprite->getRenderSubDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();

            for (auto& index : m_sprite->getRenderSubData())
            {
                renderSystem->bufferSubData(m_renderTrianglesId, index, m_sprite->getRenderVertices()[index]);
            }

            m_sprite->setRenderSubDataDirty(false);
        }
    }

    super::update(deltaTime);
}

void SpriteNode::render(RenderSystem* renderSystem)
{
    m_shader->bind();
    m_shader->setMat4("model", getAbsoluteTransform());

    if (getScene()->renderTriangles)
    {
        m_shader->setVec3("material.ambient", m_sprite->getMaterial().ambient);
        m_shader->setVec4("material.diffuse", m_sprite->getMaterial().diffuse);
        m_shader->setVec3("material.specular", m_sprite->getMaterial().specular);
        m_shader->setVec3("material.emission", m_sprite->getMaterial().emission);
        m_shader->setFloat("material.shininess", m_sprite->getMaterial().shininess);

        m_shader->setInt("material.diffuseMap", 0);
        m_shader->setInt("material.specularMap", 1);
        m_shader->setInt("material.emissionMap", 2);

        renderSystem->bindTexture(0, m_sprite->getMaterial().diffuseMap.id);
        renderSystem->bindTexture(1, m_sprite->getMaterial().specularMap.id);
        renderSystem->bindTexture(2, m_sprite->getMaterial().emissionMap.id);

        renderSystem->bindData(m_renderTrianglesId);
        renderSystem->renderTriangles();
    }

    renderSystem->unbindTexture();
    renderSystem->unbindData();

    m_shader->unbind();

    super::render(renderSystem);
}

void SpriteNode::attachSprite(std::unique_ptr<Sprite> sprite)
{
    m_sprite = std::move(sprite);
}

Sprite* SpriteNode::getSprite() const
{
    return m_sprite.get();
}
