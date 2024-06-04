#include "Select.h"

void SelectOperator::onEnter(View&)
{
}

void SelectOperator::onExit(View&)
{
}

void SelectOperator::onMouseMove(View& view, double x, double y)
{

}

void SelectOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (action == Action::Press)
    {
        auto contacts = view.raycast(x, y, FilterValue::Node);

        if (!contacts.empty())
            view.setSelected(contacts.front().node);
        else
            view.setSelected(nullptr);
    }
}

void SelectOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}
