#include "Manipulator.h"

void Manipulator::setCallback(const std::function<void(const glm::mat4&)>& inCallback)
{
    m_callback = inCallback;
}

void Manipulator::sendFeedback(const glm::mat4& deltaMatrix)
{
    auto root = dynamic_cast<Manipulator*>(getParent());
    
    glm::vec3 center = getRelativeTransform() * glm::vec4(m_centerPoint, 1.0f);

    if (root)
    {       
        for (auto& sn : root->Node::getChildren())
        {
            sn->applyRelativeTransform(glm::translate(-center));
            sn->applyRelativeTransform(deltaMatrix);
            sn->applyRelativeTransform(glm::translate(center));
        }

        if (root->m_callback)
            root->m_callback(deltaMatrix);
    }
    else
    {
        Node::applyRelativeTransform(glm::translate(-center));
        Node::applyRelativeTransform(deltaMatrix);
        Node::applyRelativeTransform(glm::translate(center));

        if (m_callback)
            m_callback(deltaMatrix);
    }
}

