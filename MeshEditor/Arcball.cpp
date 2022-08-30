#include "Arcball.h"

void ArcballOperator::onEnter(View&)
{
}

void ArcballOperator::onExit(View&)
{
}

void ArcballOperator::onMouseMove(View& view, double x, double y)
{
    if (active)
    {
        x = (x / view.getViewport().getWidth() * split - range) * view.getViewport().calcAspectRatio();
        y = (y / view.getViewport().getHeight() * split - range);

        if (click)
            click = !click;
        else
            view.getViewport().getCamera().orbit(a, { x, y, 0.0f });

        a = { x, y, 0.0f };
    }
}

void ArcballOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (action == Action::Press)
        active = click = true;
    else
        active = click = false;
}

void ArcballOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
}
