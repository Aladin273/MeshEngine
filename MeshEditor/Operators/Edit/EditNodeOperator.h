#pragma once

#include "MeshEngine/Manipulators/Triad.h"

#include "MeshEditor/Application/View.h"
#include "MeshEditor/Operators/Operator.h"

class EditNodeOperator : public Operator
{
public:
    void onEnter(View&) override;
    void onExit(View&) override;
    void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

private:	
    ButtonCode m_button = ButtonCode::Button_Left;

    Contact m_contact;
    View* m_view;
};

