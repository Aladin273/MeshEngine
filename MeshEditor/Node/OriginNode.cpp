#include "OriginNode.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEditor/Editor/View.h"

#include "ArrowNode.h"

OriginNode::OriginNode(View* view)
{
    attachNode(std::make_unique<ArrowNode>(view, MeshEngine::Settings::axisX, MeshEngine::Settings::colorRed));
    attachNode(std::make_unique<ArrowNode>(view, MeshEngine::Settings::axisY, MeshEngine::Settings::colorGreen));
    attachNode(std::make_unique<ArrowNode>(view, MeshEngine::Settings::axisZ, MeshEngine::Settings::colorBlue));
    
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

