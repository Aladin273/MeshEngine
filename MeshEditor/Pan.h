#pragma once

#include "Operator.h"
#include "View.h"

class PanOperator : public Operator
{
public:
    ~PanOperator() {}
    void onEnter(View&) override;
    void onExit(View&) override;
    void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

private:
    double u0 = 0.0;     // for left-click
    double v0 = 0.0;
    bool active = false; // track click/repeating
    bool click = true;
};

