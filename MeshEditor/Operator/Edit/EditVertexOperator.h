#pragma once

#include "MeshEditor/Operator/Operator.h"
#include "MeshEditor/Editor/View.h"

class EditVertexOperator : public Operator
{
public:
    ~EditVertexOperator() {}
    void onEnter(View&) override;
    void onExit(View&) override;
    void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

private:
    ButtonCode m_buttonArrow = ButtonCode::Button_Left;
    ButtonCode m_buttonTriad = ButtonCode::Button_Right;

    glm::vec3 m_center { 0.0f};
    glm::vec3 m_normal { 0.0f};

    HalfEdgeVertexHandle m_vh;
    Contact m_contact;
    View* m_view;
};

