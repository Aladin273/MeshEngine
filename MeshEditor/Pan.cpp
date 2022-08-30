#include "Pan.h"

void PanOperator::onEnter(View&)
{
}

void PanOperator::onExit(View&)
{
}

void PanOperator::onMouseMove(View& view, double x, double y)
{
    if (active) // Pan
    {
        x = view.getViewport().calcTargetPlaneWidth() / view.getViewport().getWidth() * x;
        y = view.getViewport().calcTargetPlaneHeight() / view.getViewport().getHeight() * y;

        if (click)
            click = !click;
        else
            view.getViewport().getCamera().pan(static_cast<float>(u0 - x), static_cast<float>(v0 - y));

        u0 = x; v0 = y;
    }
}

void PanOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (action == Action::Press)
        active = click = true;
    else
        active = click = false;
}

void PanOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}
