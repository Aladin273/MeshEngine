#include "OriginNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEditor/Editor/View.h"

#include "ArrowNode.h"

OriginNode::OriginNode(View* view)
{
    std::unique_ptr<Mesh> arrowX = Mesh::createArrow(MeshEngine::Settings::axisX, 0.06f, 0.1f, 0.02f, 0.8f, 32);
    std::unique_ptr<Mesh> arrowY = Mesh::createArrow(MeshEngine::Settings::axisY, 0.06f, 0.1f, 0.02f, 0.8f, 32);
    std::unique_ptr<Mesh> arrowZ = Mesh::createArrow(MeshEngine::Settings::axisZ, 0.06f, 0.1f, 0.02f, 0.8f, 32);

    arrowX->setMaterial(MeshEngine::Settings::materialRed);
    arrowY->setMaterial(MeshEngine::Settings::materialGreen);
    arrowZ->setMaterial(MeshEngine::Settings::materialBlue);

    attachNode(std::make_unique<ArrowNode>(view));
    attachNode(std::make_unique<ArrowNode>(view));
    attachNode(std::make_unique<ArrowNode>(view));

    dynamic_cast<ArrowNode*>(getChildren()[0].get())->attachMesh(std::move(arrowX));
    dynamic_cast<ArrowNode*>(getChildren()[1].get())->attachMesh(std::move(arrowY));
    dynamic_cast<ArrowNode*>(getChildren()[2].get())->attachMesh(std::move(arrowZ));

    m_shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "meshUnlitVertex.glsl", MeshEngine::Settings::shadersPath + "meshUnlitFragment.glsl");
    
    m_view = view;
}

OriginNode::~OriginNode()
{

}

void OriginNode::update(float deltaTime)
{
    setRelativeTransform(glm::scale(glm::vec3(m_view->getViewport().getCamera().getDistanceToTarget() * 0.15f)));
    super::update(deltaTime);
}

