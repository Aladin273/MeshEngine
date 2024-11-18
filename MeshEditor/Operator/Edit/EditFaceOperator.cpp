#include "EditFaceOperator.h"


void EditFaceOperator::onEnter(View&)
{

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
    if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Press && !view.getViewportLayer().wantCaptureGizmo())
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

        if (contacts.empty())
        {
            m_view->getViewportLayer().setViewportMode(ViewportMode::Select);
            return;
        }

        Contact& contact = contacts.front();

        m_view = &view;
        m_contact = contact;

        MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
        if (!node) return;

        const auto& table = node->getMesh()->getHalfEdgeTable();
        heds::HalfEdgeHandle heh0 = table.deref(m_contact.face).heh;
        heds::HalfEdgeHandle heh1 = table.next(heh0);
        heds::HalfEdgeHandle heh2 = table.next(heh1);
        heds::HalfEdgeHandle heh3 = table.next(heh2);

        glm::mat4 trf = node->getAbsoluteTransform();

        glm::vec3 a = trf * glm::vec4(table.getEndPoint(heh0).position, 1.0f);
        glm::vec3 b = trf * glm::vec4(table.getEndPoint(heh1).position, 1.0f);
        glm::vec3 c = trf * glm::vec4(table.getEndPoint(heh2).position, 1.0f);
        glm::vec3 d = trf * glm::vec4(table.getEndPoint(heh3).position, 1.0f);

        m_normal = glm::normalize(glm::cross(b - a, c - b));

        if (heh3 == heh0)
            m_center = (a + b + c) / 3.0f;
        else
            m_center = (a + b + c + d) / 4.0f;

        m_view->getViewportLayer().setViewportMode(ViewportMode::Translate);
        m_view->getViewportLayer().setGizmoMode(GizmoMode::World);
        m_view->getViewportLayer().setGizmoTransform(glm::translate(m_center));
        m_view->getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
            {
                MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
                if (!node) return;

                node->getMesh()->applyTransformation(m_contact.face, delta);
            });
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}