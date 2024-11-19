#pragma once

#include "MeshEditor/Operator/Operator.h"
#include "MeshEditor/Editor/View.h"

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

