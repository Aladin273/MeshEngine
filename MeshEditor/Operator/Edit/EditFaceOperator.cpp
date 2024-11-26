#include "EditFaceOperator.h"


void EditFaceOperator::onEnter(View& view)
{
    view.onSelectedChanged.addUnique(this, &EditFaceOperator::onSelectedChanged);

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

            if (mods == Modifier::Shift)
            {
                contact = Contact{ {}, contact.node->getRoot(), {}, {} };
            }

            view.setSelected(contact);
        }
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().setGizmoMode((GizmoMode)(((uint8_t)view.getViewportLayer().getGizmoMode() + 1) % (uint8_t)GizmoMode::MAX));
    }
}

void EditFaceOperator::onSelectedChanged(View& view, const Contact& selected)
{
    if (m_active)
    {
        if (selected.node)
        {
            MeshNode* node = dynamic_cast<MeshNode*>(selected.node);
            if (!node) return;

            const auto& table = node->getMesh()->getHalfEdgeTable();
            HalfEdgeHandle heh0 = table.deref(selected.face).heh;
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

            view.getViewportLayer().setGizmoSpace(GizmoSpace::World);
            view.getViewportLayer().setGizmoTransform(glm::translate(center));

            view.getViewportLayer().setGizmoCallback([&](const glm::mat4& transform, const glm::mat4& delta)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(selected.node);
                    if (!node) return;

                    node->getMesh()->applyTransformation(selected.face, delta);
                });
        }
        else
        {
            view.getViewportLayer().setGizmoVisible(false);
        }
    }
}
