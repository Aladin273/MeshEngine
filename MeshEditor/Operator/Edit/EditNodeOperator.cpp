#include "EditNodeOperator.h"

void EditNodeOperator::onEnter(View& view)
{

}

void EditNodeOperator::onExit(View& view)
{
    m_contact = Contact{};
    view.setSelected(nullptr);
    view.getViewportLayer().setVisible(false);
}

void EditNodeOperator::onMouseMove(View& view, double x, double y)
{

}

void EditNodeOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (button == ButtonCode::Button_Left && action == Action::Press && !view.getViewportLayer().wantCaptureGizmo())
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

        if (contacts.empty())
        {
            m_contact = Contact{};
            view.setSelected(nullptr);
            view.getViewportLayer().setVisible(false);
        }
        else
        {
            m_contact = contacts.front();

            if (mods == Modifier::Shift)
                m_contact.node = m_contact.node->getRoot();

            Node* node = m_contact.node;
            if (!node) return;

            view.setSelected(node);
            view.getViewportLayer().setVisible(true);
            view.getViewportLayer().setGizmoMode(GizmoMode::World);
            view.getViewportLayer().setGizmoTransform(node->getAbsoluteTransform());
            view.getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
                {
                    Node* node = m_contact.node;
                    if (!node) return;

                    glm::mat4 parentInverse = node->getParent() ? glm::inverse(node->getParent()->getAbsoluteTransform()) : glm::mat4(1.0f);
                    node->setRelativeTransform(parentInverse * transform);
                });
        }
    }
}

void EditNodeOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().setViewportMode((ViewportMode)(((uint8_t)view.getViewportLayer().getViewportMode() + 1) % (uint8_t)ViewportMode::MAX));
    }
}