#include "EditFaceOperator.h"


void EditFaceOperator::onEnter(View& view)
{
    view.onSelectedChanged.addUnique(this, &EditFaceOperator::onSelectedChanged);

    view.getViewportLayer().setGizmoMode(GizmoMode::Translate);
    view.getViewportLayer().setGizmoSpace(GizmoSpace::World);

    m_active = true;
}

void EditFaceOperator::onExit(View& view)
{
    view.setSelected(Contact{});
    view.getViewportLayer().setGizmoVisible(false);

    m_active = false;
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
            view.setSelected(Contact{});
            view.getViewportLayer().setGizmoVisible(false);
        }
        else
        {
            Contact contact = contacts.front();

            MeshNode* node = dynamic_cast<MeshNode*>(contact.node);
            if (!node) return;

            const auto& table = node->getMesh()->getHalfEdgeTable();
            HalfEdgeHandle heh0 = table.deref(contact.face).heh;
            HalfEdgeHandle heh1 = table.next(heh0);
            HalfEdgeHandle heh2 = table.next(heh1);
            HalfEdgeHandle heh3 = table.next(heh2);

            glm::mat4 trf = node->getAbsoluteTransform();

            glm::vec3 a = trf * glm::vec4(table.getEndPoint(heh0).position, 1.0f);
            glm::vec3 b = trf * glm::vec4(table.getEndPoint(heh1).position, 1.0f);
            glm::vec3 c = trf * glm::vec4(table.getEndPoint(heh2).position, 1.0f);
            glm::vec3 d = trf * glm::vec4(table.getEndPoint(heh3).position, 1.0f);

            glm::vec3 center = heh3 == heh0 ? (a + b + c) / 3.0f : (a + b + c + d) / 4.0f;

            view.getViewportLayer().setGizmoVisible(true);
            view.getViewportLayer().setGizmoTransform(glm::translate(center));
            view.getViewportLayer().setGizmoCallback([contact, &view](const glm::mat4& transform, const glm::mat4& delta, const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(contact.node);
                    if (!node) return;

                    glm::mat4 inverse = glm::inverse(node->getAbsoluteTransform());

                    glm::mat4 localTranslation = glm::translate(glm::vec3(inverse * glm::vec4(translation, 1.f)));
                    glm::mat4 localRotation = glm::mat4_cast(glm::quat_cast(inverse) * glm::quat(glm::radians(rotation)));
                    glm::mat4 localScale = glm::scale(glm::vec3(inverse * glm::vec4(scale, 0.f)));

                    node->getMesh()->applyTransformation(contact.face, localTranslation);
                });
        }
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}

void EditFaceOperator::onSelectedChanged(View& view, const Contact& selected)
{
    if (m_active && selected.node)
    {
        view.setSelected(Contact{});
    }
}
