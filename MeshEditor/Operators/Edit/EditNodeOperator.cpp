#include "EditNodeOperator.h"

void EditNodeOperator::onEnter(View& view)
{
	m_idle = true;
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
    if (button == m_button && action == Action::Press && m_idle == true)
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

        if (contacts.empty())
            return;
        
        Contact& contact = contacts.front();
        Node* node = contact.node;

        if (view.getViewportLayer().wantCaptureGizmo()) // Selected Manipulator
        {
            m_idle = false;
            m_edit = true;
        }
        else                                            // Selected Node
        {
            m_view = &view;
            m_contact = contact;          
            
            const auto& bbox = m_contact.node->getMesh()->getBoundingBox();
            m_center = m_contact.node->calcAbsoluteTransform() * glm::vec4((bbox.min + bbox.max) / 2.f, 1.0f);

            m_view->getViewportLayer().setViewportMode(ViewportMode::Universal);
            m_view->getViewportLayer().setGizmoMode(GizmoMode::World);
            m_view->getViewportLayer().setGizmoTransform(glm::translate(m_center));
            m_view->getViewportLayer().setGizmoCallback([&](const glm::mat4& delta)
                {
                    m_contact.node->applyRelativeTransform(delta);
                });
        }
    }
    else if (button == m_button && action == Action::Release && m_idle == false)
    {
        m_idle = true;
        m_edit = false;
    }
}

void EditNodeOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
}