#include "EditFaceOperator.h"


void EditFaceOperator::onEnter(View&)
{
    m_idle = true;
}

void EditFaceOperator::onExit(View&)
{
    m_view->getViewportLayer().setViewportMode(ViewportMode::Select);
}

void EditFaceOperator::onMouseMove(View& view, double x, double y)
{

}

void EditFaceOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Press && m_idle == true) // Idle On
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

            const auto& table = m_contact.node->getMesh()->getHalfEdgeTable();
            heds::HalfEdgeHandle heh0 = table.deref(m_contact.face).heh;
            heds::HalfEdgeHandle heh1 = table.next(heh0);
            heds::HalfEdgeHandle heh2 = table.next(heh1);
            heds::HalfEdgeHandle heh3 = table.next(heh2);

            glm::mat4 trf = m_contact.node->calcAbsoluteTransform();

            glm::vec3 a = trf * glm::vec4(table.getEndPoint(heh0).position, 1.0f);
            glm::vec3 b = trf * glm::vec4(table.getEndPoint(heh1).position, 1.0f);
            glm::vec3 c = trf * glm::vec4(table.getEndPoint(heh2).position, 1.0f);
            glm::vec3 d = trf * glm::vec4(table.getEndPoint(heh3).position, 1.0f);

            m_normal = glm::normalize(glm::cross(b - a, c - b));

            if (heh3 == heh0)
                m_center = (a + b + c) / 3.0f;
            else
                m_center = (a + b + c + d) / 4.0f;

            m_view->getViewportLayer().setViewportMode(ViewportMode::Universal);
            m_view->getViewportLayer().setGizmoMode(GizmoMode::World);
            m_view->getViewportLayer().setGizmoTransform(glm::translate(m_center));
            m_view->getViewportLayer().setGizmoCallback([&](const glm::mat4& delta)
                {
                    m_contact.node->getMesh()->applyTransformation(m_contact.face, delta);
                });
        }
    }
    else if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Release && m_idle == false) // Idle Off
    {
        m_idle = true;
        m_edit = false;
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}