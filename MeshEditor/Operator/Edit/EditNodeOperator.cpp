#include "EditNodeOperator.h"

void EditNodeOperator::onEnter(View& view)
{
    view.onSelectedChanged.addUnique(this, &EditNodeOperator::onSelectedChanged);
    
    view.getViewportLayer().setGizmoMode(GizmoMode::Translate);
    view.getViewportLayer().setGizmoSpace(GizmoSpace::World);
    
    m_active = true;
}

void EditNodeOperator::onExit(View& view)
{
    view.setSelected(Contact{});
    view.getViewportLayer().setGizmoVisible(false);
    
    m_active = false;
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
            view.setSelected(Contact{});
            view.getViewportLayer().setGizmoVisible(false);
        }
        else
        {
            Contact contact = contacts.front();

            if (mods == Modifier::Shift)
            {
                contact = Contact{ {}, contact.node->getRoot(), {}, {} };
            }

            view.setSelected(contact);
        }
    }
}

void EditNodeOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().switchGizmoMode();
    }
}

void EditNodeOperator::onSelectedChanged(View& view, const Contact& selected)
{
    if (m_active)
    {
        if (selected.node)
        {
            view.getViewportLayer().setGizmoVisible(true);
            view.getViewportLayer().setGizmoTransform(selected.node->getAbsoluteTransform());
            view.getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
                {
                    Node* node = selected.node;
                    if (!node) return;

                    glm::mat4 parentInverse = node->getParent() ? glm::inverse(node->getParent()->getAbsoluteTransform()) : glm::mat4(1.0f);
                    node->setRelativeTransform(parentInverse * transform);
                });
        }
        else
        {
            view.getViewportLayer().setGizmoVisible(false);
        }
    }
}