#pragma once
#include "Operator.h"
#include "View.h"

class DeleteFaceOperator : public Operator
{
public:
    ~DeleteFaceOperator() {}
    void onEnter(View&) override;
    void onExit(View&) override;
    void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;
};

