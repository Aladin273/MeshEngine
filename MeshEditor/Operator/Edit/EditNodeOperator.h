#pragma once

#include "MeshEditor/Operator/Operator.h"
#include "MeshEditor/Editor/View.h"

class EditNodeOperator : public Operator
{
public:
    virtual void onEnter(View&) override;
    virtual void onExit(View&) override;
    virtual void onMouseMove(View& view, double x, double y) override;
    virtual void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    virtual void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

private:
    void onSelectedChanged(View& view, const Contact& selected);

private:
    bool m_active = false;
};

