#pragma once

#include "MeshEngine/Manipulators/Triad.h"

#include "MeshEditor/Application/View.h"
#include "MeshEditor/Operators/Operator.h"

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
    void init();
    void clear();
    void setupTriad();
    void setupArrow();

    ButtonCode m_buttonArrow = ButtonCode::Button_Left;
    ButtonCode m_buttonTriad = ButtonCode::Button_Right;

    bool m_edit = false;
    bool m_idle = false;

    glm::vec3 m_center { 0.0f};
    glm::vec3 m_normal { 0.0f};

    Utils::VertexHandle m_vh;
    Contact m_contact;
    View* m_view;
    Triad* m_triad;
    Manipulator* m_manipulator;
};

