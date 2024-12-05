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
    glm::vec3 position = m_view->getViewport().unproject(m_view->getViewport().getWidth() * 0.1f, m_view->getViewport().getHeight() * 0.1f, 0.5f);
    setRelativeTransform(glm::translate(position) * glm::scale(glm::vec3(0.025f)));

    super::update(deltaTime);
}

