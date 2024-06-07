#include "EditVertexOperator.h"

void EditVertexOperator::onEnter(View&)
{
    m_idle = true;
}

void EditVertexOperator::onExit(View&)
{
    m_view->getViewportLayer().setViewportMode(ViewportMode::Select);
}

void EditVertexOperator::onMouseMove(View& view, double x, double y)
{

}

void EditVertexOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
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
        else
        {
            m_view = &view;
            m_contact = contact;

            const auto& table = m_contact.node->getMesh()->getHalfEdgeTable();
            heds::HalfEdgeHandle start_heh = table.deref(m_contact.face).heh;
            heds::HalfEdgeHandle next_heh = start_heh;
            std::vector<glm::vec3> normals;
            
            glm::mat4 trf = m_contact.node->calcAbsoluteTransform();
            glm::vec3 point = glm::inverse(trf) * glm::vec4(m_contact.point, 1.0f);

            float min = glm::length(table.getEndPoint(start_heh).position - point);
            m_vh = table.deref(start_heh).dst;

            // Find needed vertex
            do
            {
                float length = glm::length(table.getEndPoint(next_heh).position - point);

                if (length < min)
                {
                    min = length;
                    m_vh = table.deref(next_heh).dst;
                }
                next_heh = table.next(next_heh);
            } while (next_heh != start_heh);

            start_heh = table.deref(m_vh).heh;
            next_heh = start_heh;

            // Find adjacent normals
            do
            {
                heds::HalfEdgeHandle heh0 = next_heh;
                heds::HalfEdgeHandle heh1 = table.next(heh0);
                heds::HalfEdgeHandle heh2 = table.next(heh1);

                glm::vec3 a = table.getEndPoint(heh0).position;
                glm::vec3 b = table.getEndPoint(heh1).position;
                glm::vec3 c = table.getEndPoint(heh2).position;

                normals.push_back(glm::normalize(glm::cross(b - a, c - b)));
                next_heh = table.next(table.twin(next_heh));
            } while (next_heh != start_heh);

            m_normal = glm::vec3(0.0f);

            for (auto& normal : normals)
                m_normal += normal;

            m_center = trf * glm::vec4(table.getPoint(m_vh).position, 1.0f);
            m_normal = glm::normalize(m_normal / static_cast<float>(normals.size()));

            m_view->getViewportLayer().setViewportMode(ViewportMode::Universal);
            m_view->getViewportLayer().setGizmoMode(GizmoMode::World);
            m_view->getViewportLayer().setGizmoTransform(glm::translate(m_center));
            m_view->getViewportLayer().setGizmoCallback([&](const glm::mat4& delta)
                {
                    m_contact.node->getMesh()->applyTransformation(m_vh, delta);
                });
        }
    }
    else if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Release && m_idle == false) // Idle Off
    {
        m_idle = true;
        m_edit = false;
    }
}

void EditVertexOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}