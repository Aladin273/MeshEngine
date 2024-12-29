#include "DirLightNode.h"

#include "MeshEngine/Scene/Scene.h"

DirLightNode::DirLightNode()
{
    m_name = "DirLightNode";
    m_renderMesh = Mesh::createArrow(glm::vec3(0.f, -1.f, 0.f), 0.375f, 0.75f, 0.1875f, 2.25f, 16);
}

DirLightNode::~DirLightNode()
{

}

void DirLightNode::start()
{
    if (Scene* scene = getScene())
    {
        m_id = scene->lightsUniform.numDirLights;
        scene->lightsUniform.numDirLights += 1;
    }

    super::start();
}

void DirLightNode::end()
{
    if (Scene* scene = getScene())
    {
        m_id = 0;
        scene->lightsUniform.numDirLights -= 1;
    }

    super::end();
}

void DirLightNode::update(float deltaTime)
{
    if (Scene* scene = getScene())
    {
        m_light.direction = getAbsoluteTransform() * glm::vec4(0.f, -1.f, 0.f, 0.f);
        scene->lightsUniform.dirLights[m_id] = m_light;

        m_renderColor = m_light.diffuse;
    }

    super::update(deltaTime);
}

void DirLightNode::render(RenderSystem* renderSystem)
{
    super::render(renderSystem);
}
