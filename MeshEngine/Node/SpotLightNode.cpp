#include "SpotLightNode.h"

#include "MeshEngine/Scene/Scene.h"

SpotLightNode::SpotLightNode()
{
    m_name = "SpotLightNode";
    m_renderMesh = Mesh::createCone(glm::vec3(0.f, 1.f, 0.f), 1.f, 3.f, 32);
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
        scene->lightsUniform.spotLights[m_id].position = getAbsoluteTransform() * glm::vec4(0.f, 0.f, 0.f, 1.f);
        scene->lightsUniform.spotLights[m_id].direction = getAbsoluteTransform() * glm::vec4(0.f, -1.f, 0.f, 0.f);
        
        scene->lightsUniform.spotLights[m_id].ambient = ambient;
        scene->lightsUniform.spotLights[m_id].diffuse = diffuse;
        scene->lightsUniform.spotLights[m_id].specular = specular;
        
        scene->lightsUniform.spotLights[m_id].cutOff = cutOff;
        scene->lightsUniform.spotLights[m_id].outerCutOff = outerCutOff;
        scene->lightsUniform.spotLights[m_id].constant = constant;
        scene->lightsUniform.spotLights[m_id].linear = linear;
        scene->lightsUniform.spotLights[m_id].quadratic = quadratic;

        m_renderColor = diffuse;
    }

    super::update(deltaTime);
}

void SpotLightNode::render(RenderSystem* renderSystem)
{
    super::render(renderSystem);
}
