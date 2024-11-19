#include "EditNodeOperator.h"

void EditNodeOperator::onEnter(View& view)
{

}

void EditNodeOperator::onExit(View& view)
{
    m_view->getViewportLayer().setViewportMode(ViewportMode::Select);
}

void EditNodeOperator::onMouseMove(View& view, double x, double y)
{

}

void EditNodeOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (button == m_button && action == Action::Press && !view.getViewportLayer().wantCaptureGizmo())
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

        if (contacts.empty())
        {
            m_view->getViewportLayer().setViewportMode(ViewportMode::Select);
            return;
        }
        
        Contact& contact = contacts.front();

        if (mods == Modifier::Shift)
        {
            while (contact.node->getParent())
                contact.node = contact.node->getParent();
        }

        m_view = &view;
        m_contact = contact;

        Node* node = m_contact.node;
        if (!node) return;

        if (MeshNode* meshNode = dynamic_cast<MeshNode*>(node))
        {
            // Re-calculate center
            glm::vec3 center = glm::vec4((meshNode->getBoundingBox().min + meshNode->getBoundingBox().max) / 2.0f, 1.0f);

            if (glm::any(glm::notEqual(glm::vec3(0.0f), center, 1e-8)))
            {
                auto& table = meshNode->getMesh()->getHalfEdgeTable();

                for (auto& vertex : table.getVertices())
                    vertex.data.position -= center;

                meshNode->getMesh()->update();
                meshNode->applyRelativeTransform(glm::translate(center));

                meshNode->end();
                meshNode->start();
            }
        }

        m_view->getViewportLayer().setViewportMode(ViewportMode::Translate);
        m_view->getViewportLayer().setGizmoMode(GizmoMode::World);
        m_view->getViewportLayer().setGizmoTransform(node->getAbsoluteTransform());
        m_view->getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
            {
                Node* node = m_contact.node;
                if (!node) return;

                glm::mat4 parentInverse = node->getParent() ? glm::inverse(node->getParent()->getAbsoluteTransform()) : glm::mat4(1.0f);
                node->setRelativeTransform(parentInverse * transform);
            });
    }
}

void EditNodeOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        if (m_view->getViewportLayer().getViewportMode() == ViewportMode::Translate)
            m_view->getViewportLayer().setViewportMode(ViewportMode::Rotate);
        else if (m_view->getViewportLayer().getViewportMode() == ViewportMode::Rotate)
            m_view->getViewportLayer().setViewportMode(ViewportMode::Scale);
        else if (m_view->getViewportLayer().getViewportMode() == ViewportMode::Scale)
            m_view->getViewportLayer().setViewportMode(ViewportMode::Translate);
    }
}