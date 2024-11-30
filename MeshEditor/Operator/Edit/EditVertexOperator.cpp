#include "EditVertexOperator.h"

#include <glm/gtx/matrix_decompose.hpp>

void EditVertexOperator::onEnter(View& view)
{
    view.onSelectedChanged.addUnique(this, &EditVertexOperator::onSelectedChanged);
    
    view.getViewportLayer().setGizmoMode(GizmoMode::Translate);
    view.getViewportLayer().setGizmoSpace(GizmoSpace::World);

    m_active = true;
}

void EditVertexOperator::onExit(View& view)
{
    view.setSelected(Contact{});
    view.getViewportLayer().setGizmoVisible(false);

    m_active = false;
}

void EditVertexOperator::onMouseMove(View& view, double x, double y)
{

}

void EditVertexOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
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

            MeshNode* node = dynamic_cast<MeshNode*>(contact.node);
            if (!node) return;

            const auto& table = node->getMesh()->getHalfEdgeTable();
            HalfEdgeHandle start_heh = table.deref(contact.face).heh;
            HalfEdgeHandle next_heh = start_heh;
            std::vector<glm::vec3> normals;

            glm::mat4 trf = contact.node->getAbsoluteTransform();
            glm::vec3 point = glm::inverse(trf) * glm::vec4(contact.point, 1.0f);

            float min = glm::length(table.getEndPoint(start_heh).position - point);
            HalfEdgeVertexHandle vh = table.deref(start_heh).dst;

            // Find needed vertex
            do
            {
                float length = glm::length(table.getEndPoint(next_heh).position - point);

                if (length < min)
                {
                    min = length;
                    vh = table.deref(next_heh).dst;
                }
                next_heh = table.next(next_heh);
            } while (next_heh != start_heh);

            glm::vec3 center = trf * glm::vec4(table.getPoint(vh).position, 1.0f);

            view.getViewportLayer().setGizmoVisible(true);
            view.getViewportLayer().setGizmoTransform(glm::translate(center));
            view.getViewportLayer().setGizmoCallback([contact, vh, &view](const glm::mat4& transform, const glm::mat4& delta)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(contact.node);
                    if (!node) return;

                    glm::vec3 translation, scale, skew;
                    glm::vec4 perspective; glm::quat rotation;

                    glm::decompose(delta, scale, rotation, translation, skew, perspective);

                    glm::mat4 absolute = node->getAbsoluteTransform();
                    glm::mat4 inverse = glm::inverse(absolute);

                    glm::mat4 relativeDelta;

                    switch (view.getViewportLayer().getGizmoMode())
                    {
                    case GizmoMode::Translate: relativeDelta = inverse * glm::translate(translation) * absolute; break;
                    case GizmoMode::Rotate: relativeDelta = glm::mat3(inverse) * glm::mat3_cast(rotation) * glm::mat3(absolute); break;
                    case GizmoMode::Scale: relativeDelta = glm::mat3(inverse) * glm::mat3(glm::scale(scale)) * glm::mat3(absolute); break;
                    }

                    node->getMesh()->applyTransformation(vh, relativeDelta);
                });
        }
    }
}

void EditVertexOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}

void EditVertexOperator::onSelectedChanged(View& view, const Contact& selected)
{
    if (m_active && selected.node)
    {
        view.setSelected(Contact{});
    }
}
