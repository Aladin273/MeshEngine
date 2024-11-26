#include "EditFaceOperator.h"


void EditFaceOperator::onEnter(View& view)
{

}

void EditFaceOperator::onExit(View& view)
{
    m_contact = Contact{};
    view.getViewportLayer().setVisible(false);
}

void EditFaceOperator::onMouseMove(View& view, double x, double y)
{

}

void EditFaceOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (button == ButtonCode::Button_Left && action == Action::Press && !view.getViewportLayer().wantCaptureGizmo())
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

        if (contacts.empty())
        {
            m_contact = Contact{};
            view.getViewportLayer().setVisible(false);
        }
        else
        {
            m_contact = contacts.front();

            MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
            if (!node) return;

            const auto& table = node->getMesh()->getHalfEdgeTable();
            HalfEdgeHandle heh0 = table.deref(m_contact.face).heh;
            HalfEdgeHandle heh1 = table.next(heh0);
            HalfEdgeHandle heh2 = table.next(heh1);
            HalfEdgeHandle heh3 = table.next(heh2);

            glm::mat4 trf = node->getAbsoluteTransform();

            glm::vec3 a = trf * glm::vec4(table.getEndPoint(heh0).position, 1.0f);
            glm::vec3 b = trf * glm::vec4(table.getEndPoint(heh1).position, 1.0f);
            glm::vec3 c = trf * glm::vec4(table.getEndPoint(heh2).position, 1.0f);
            glm::vec3 d = trf * glm::vec4(table.getEndPoint(heh3).position, 1.0f);

            glm::vec3 center = heh3 == heh0 ? (a + b + c) / 3.0f : (a + b + c + d) / 4.0f;

            view.getViewportLayer().setVisible(true);
            view.getViewportLayer().setGizmoMode(GizmoMode::World);
            view.getViewportLayer().setGizmoTransform(glm::translate(m_center));
            view.getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
                    if (!node) return;

                    node->getMesh()->applyTransformation(m_contact.face, delta);
                });
        }
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().setViewportMode((ViewportMode)(((uint8_t)view.getViewportLayer().getViewportMode() + 1) % (uint8_t)ViewportMode::MAX));
    }
}