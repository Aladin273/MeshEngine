#include "SpotLightNode.h"

#include "MeshEngine/Scene/Scene.h"

SpotLightNode::SpotLightNode()
{
    m_name = "SpotLightNode";
    m_renderMesh = Mesh::createCone(glm::vec3(0.f, 1.f, 0.f), 0.75f, 2.f, 16);
}

SpotLightNode::~SpotLightNode()
{

}

void SpotLightNode::start()
{
    if (Scene* scene = getScene())
    {
        m_id = scene->lightsUniform.numSpotLights;
        scene->lightsUniform.numSpotLights += 1;
    }

    super::start();
}

void SpotLightNode::end()
{
    if (Scene* scene = getScene())
    {
        m_id = 0;
        scene->lightsUniform.numSpotLights -= 1;
    }

    super::end();
}

void SpotLightNode::update(float deltaTime)
{
    if (Scene* scene = getScene())
    {
        m_light.position = getAbsoluteTransform() * glm::vec4(0.f, 0.f, 0.f, 1.f);
        m_light.direction = getAbsoluteTransform() * glm::vec4(0.f, -1.f, 0.f, 0.f);
        scene->lightsUniform.spotLights[m_id] = m_light;

        m_renderColor = m_light.diffuse;
    }

    super::update(deltaTime);
}

void SpotLightNode::render(RenderSystem* renderSystem)
{
    super::render(renderSystem);
}
