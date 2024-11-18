#include "MeshNode.h"

#include "MeshEngine/Scene/Scene.h"

MeshNode::MeshNode()
{
    m_name = "MeshNode";
    m_mesh = std::make_unique<Mesh>(heds::HalfEdgeTable<Vertex>());
    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "meshLitVertex.glsl", MeshEngine::Settings::shadersPath + "meshLitFragment.glsl");
}

MeshNode::~MeshNode()
{

}

const BoundingBox& MeshNode::getBoundingBox() const
{
    return m_mesh->getBoundingBox();
}

void MeshNode::start()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_mesh->getRenderDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();

            m_renderTrianglesId = renderSystem->bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderTriangles());
            m_renderLinesId = renderSystem->bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderLines());
            m_renderHolesId = renderSystem->bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderHoles());
            m_renderBoundariesId = renderSystem->bufferData(m_mesh->getRenderVertices(), m_mesh->getRenderBoundaries());

            Material material = m_mesh->getMaterial();
            material.diffuseMap.id = renderSystem->bufferTexture(material.diffuseMap.path);
            material.specularMap.id = renderSystem->bufferTexture(material.specularMap.path);
            material.emissionMap.id = renderSystem->bufferTexture(material.emissionMap.path);
            m_mesh->setMaterial(material);

            m_mesh->setRenderDataDirty(false);
        }
    }

    super::start();
}

void MeshNode::end()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        RenderSystem* renderSystem = getScene()->getRenderSystem();

        renderSystem->unbufferData(m_renderTrianglesId);
        renderSystem->unbufferData(m_renderLinesId);
        renderSystem->unbufferData(m_renderHolesId);
        renderSystem->unbufferData(m_renderBoundariesId);

        renderSystem->unbufferTexture(m_mesh->getMaterial().diffuseMap.id);
        renderSystem->unbufferTexture(m_mesh->getMaterial().specularMap.id);
        renderSystem->unbufferTexture(m_mesh->getMaterial().emissionMap.id);

        m_mesh->setRenderDataDirty(true);
    }

    super::end();
}

void MeshNode::update(float deltaTime)
{
    if (getScene() && getScene()->getRenderSystem())
    {
        if (m_mesh->getRenderSubDataDirty())
        {
            RenderSystem* renderSystem = getScene()->getRenderSystem();

            for (auto& index : m_mesh->getRenderSubData())
            {
                renderSystem->bufferSubData(m_renderTrianglesId, index, m_mesh->getRenderVertices()[index]);
                renderSystem->bufferSubData(m_renderLinesId, index, m_mesh->getRenderVertices()[index]);
                renderSystem->bufferSubData(m_renderHolesId, index, m_mesh->getRenderVertices()[index]);
                renderSystem->bufferSubData(m_renderBoundariesId, index, m_mesh->getRenderVertices()[index]);
            }

            m_mesh->setRenderSubDataDirty(false);
        }
    }

    super::update(deltaTime);
}

void MeshNode::render(RenderSystem* renderSystem)
{
    m_shader->bind();
    m_shader->setMat4("model", getAbsoluteTransform());

    if (renderTriangles)
    {
        m_shader->setVec3("material.ambient", getMesh()->getMaterial().ambient);
        m_shader->setVec3("material.diffuse", glm::vec3(getMesh()->getMaterial().diffuse));
        m_shader->setVec3("material.specular", getMesh()->getMaterial().specular);
        m_shader->setVec3("material.emission", getMesh()->getMaterial().emission);
        m_shader->setFloat("material.shininess", getMesh()->getMaterial().shininess);

        m_shader->setInt("material.diffuseMap", 0);
        m_shader->setInt("material.specularMap", 1);
        m_shader->setInt("material.emissionMap", 2);

        renderSystem->bindTexture(0, getMesh()->getMaterial().diffuseMap.id);
        renderSystem->bindTexture(1, getMesh()->getMaterial().specularMap.id);
        renderSystem->bindTexture(2, getMesh()->getMaterial().emissionMap.id);

        renderSystem->bindData(m_renderTrianglesId);
        renderSystem->renderTriangles();
    }

    if (renderLines)
    {
        m_shader->setVec3("material.ambient", glm::vec3(0.f));
        m_shader->setVec3("material.diffuse", glm::vec3(0.f));
        m_shader->setVec3("material.specular", glm::vec3(0.f));
        m_shader->setVec3("material.emission", glm::vec3(0.f));
        m_shader->setFloat("material.shininess", 0);

        renderSystem->bindData(m_renderLinesId);

        renderSystem->setLineSize(1.0f);
        renderSystem->renderLines();
    }

    if (renderHoles)
    {
        m_shader->setVec3("material.ambient", glm::vec3(0.f));
        m_shader->setVec3("material.diffuse", glm::vec3(0.f));
        m_shader->setVec3("material.specular", glm::vec3(0.f));
        m_shader->setVec3("material.emission", glm::vec3(0.75f, 0.25f, 0.25f));
        m_shader->setFloat("material.shininess", 0);

        renderSystem->bindData(m_renderHolesId);

        renderSystem->setLineSize(4.0f);
        renderSystem->renderLines();
    }

    if (renderBoundaries)
    {
        m_shader->setVec3("material.ambient", glm::vec3(0.f));
        m_shader->setVec3("material.diffuse", glm::vec3(0.f));
        m_shader->setVec3("material.specular", glm::vec3(0.f));
        m_shader->setVec3("material.emission", glm::vec3(0.75f, 0.25f, 0.25f));
        m_shader->setFloat("material.shininess", 0);

        renderSystem->bindData(m_renderBoundariesId);
        renderSystem->renderTriangles();
    }

    renderSystem->unbindTexture();
    renderSystem->unbindData();

    m_shader->unbind();

    super::render(renderSystem);
}

void MeshNode::attachMesh(std::unique_ptr<Mesh> mesh)
{
	m_mesh = std::move(mesh);
}

Mesh* MeshNode::getMesh() const
{
	return m_mesh.get();
}
