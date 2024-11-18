#include "DirLightNode.h"

#include "MeshEngine/Scene/Scene.h"

DirLightNode::DirLightNode()
{
    m_name = "DirLightNode";
    m_renderMesh = Mesh::createArrow(glm::vec3(0.f, -1.f, 0.f), 0.5f, 1.0f, 0.25f, 3.f, 32);
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
        scene->lightsUniform.dirLights[m_id].direction = getAbsoluteTransform() * glm::vec4(0.f, -1.f, 0.f, 0.f);
        
        scene->lightsUniform.dirLights[m_id].ambient = ambient;
        scene->lightsUniform.dirLights[m_id].diffuse = diffuse;
        scene->lightsUniform.dirLights[m_id].specular = specular;

        m_renderColor = diffuse;
    }

    super::update(deltaTime);
}

void DirLightNode::render(RenderSystem* renderSystem)
{
    super::render(renderSystem);
}
