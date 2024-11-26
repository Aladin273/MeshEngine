#include "EditVertexOperator.h"

void EditVertexOperator::onEnter(View& view)
{

}

void EditVertexOperator::onExit(View& view)
{
    m_contact = Contact{};
    view.getViewportLayer().setVisible(false);
}

void EditVertexOperator::onMouseMove(View& view, double x, double y)
{

}

void EditVertexOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{   
    if (button == ButtonCode::Button_Left && action == Action::Press && !view.getViewportLayer().wantCaptureGizmo())
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

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
            HalfEdgeHandle start_heh = table.deref(m_contact.face).heh;
            HalfEdgeHandle next_heh = start_heh;
            std::vector<glm::vec3> normals;

            glm::mat4 trf = m_contact.node->getAbsoluteTransform();
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

            glm::vec3 center = trf * glm::vec4(table.getPoint(m_vh).position, 1.0f);

            view.getViewportLayer().setVisible(true);
            view.getViewportLayer().setGizmoMode(GizmoMode::World);
            view.getViewportLayer().setGizmoTransform(glm::translate(center));
            view.getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
                    if (!node) return;

                    node->getMesh()->applyTransformation(m_vh, delta);
                });
        }
    }
}

void EditVertexOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().setViewportMode((ViewportMode)(((uint8_t)view.getViewportLayer().getViewportMode() + 1) % (uint8_t)ViewportMode::MAX));
    }
}