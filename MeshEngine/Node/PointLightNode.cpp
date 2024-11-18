#include "PointLightNode.h"

#include "MeshEngine/Scene/Scene.h"

PointLightNode::PointLightNode()
{
    m_name = "PointLightNode";
    m_renderMesh = Mesh::createCube(glm::vec3(0.f), 1.0f);
}

PointLightNode::~PointLightNode()
{

}

void PointLightNode::start()
{
    if (Scene* scene = getScene())
    {
        m_id = scene->lightsUniform.numPointLights;
        scene->lightsUniform.numPointLights += 1;
    }

    super::start();
}

void PointLightNode::end()
{
    if (Scene* scene = getScene())
    {
        m_id = 0;
        scene->lightsUniform.numPointLights -= 1;
    }

    super::end();
}

void PointLightNode::update(float deltaTime)
{
    if (Scene* scene = getScene())
    {
        scene->lightsUniform.pointLights[m_id].position = getAbsoluteTransform() * glm::vec4(0.f, 0.f, 0.f, 1.f);
        
        scene->lightsUniform.pointLights[m_id].ambient = ambient;
        scene->lightsUniform.pointLights[m_id].diffuse = diffuse;
        scene->lightsUniform.pointLights[m_id].specular = specular;

        scene->lightsUniform.pointLights[m_id].constant = constant;
        scene->lightsUniform.pointLights[m_id].linear = linear;
        scene->lightsUniform.pointLights[m_id].quadratic = quadratic;

        m_renderColor = diffuse;
    }

    super::update(deltaTime);
}

void PointLightNode::render(RenderSystem* renderSystem)
{
    super::render(renderSystem);
}
