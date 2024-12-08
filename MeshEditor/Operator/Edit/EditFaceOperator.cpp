#include "EditFaceOperator.h"
#include "MeshEngine/Misc/DrawHelper.h"

#include <glm/gtx/matrix_decompose.hpp>

void EditFaceOperator::onEnter(View& view)
{
    view.onSelectedChanged.addUnique(this, &EditFaceOperator::onSelectedChanged);

    view.getViewportLayer().setGizmoMode(GizmoMode::Translate);
    view.getViewportLayer().setGizmoSpace(GizmoSpace::World);

    m_contact = Contact{};
    m_active = true;
}

void EditFaceOperator::onExit(View& view)
{
    view.setSelected(Contact{});
    view.getViewportLayer().setGizmoVisible(false);

    m_contact = Contact{};
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

            m_contact = Contact{};
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

            view.getViewportLayer().setGizmoVisible(true);
            view.getViewportLayer().setGizmoTransform(glm::translate(center));
            view.getViewportLayer().setGizmoCallback([this, &view](const glm::mat4& transform, const glm::mat4& delta)
                {
                    MeshNode* node = dynamic_cast<MeshNode*>(m_contact.node);
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

                    node->getMesh()->applyTransformation(m_contact.face, relativeDelta);
                });
        }
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (key == KeyCode::Space && action == Action::Press)
    {
        view.getViewportLayer().switchGizmoMode();

        if (view.getViewportLayer().getGizmoMode() == GizmoMode::Select)
            view.getViewportLayer().setGizmoMode(GizmoMode::Translate);
    }
}

void EditFaceOperator::onUpdate(float deltaTime)
{

}

void EditFaceOperator::onRender(RenderSystem* renderSystem)
{
    if (m_active)
    {
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

        if (heh3 == heh0)
        {
            MeshEngine::DrawHelper::drawDebugTriangle(renderSystem, a, b, c, glm::vec4(1.f, 1.0, 0.f, 0.5f), true);
        }
        else
        {
            MeshEngine::DrawHelper::drawDebugQuad(renderSystem, a, b, c, d, glm::vec4(1.f, 1.0, 0.f, 0.5f), true);
        }
    }
}

void EditFaceOperator::onSelectedChanged(View& view, const Contact& selected)
{
    if (m_active && selected.node)
    {
        view.setSelected(Contact{});
    }
}
