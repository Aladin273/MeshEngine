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
    if (action == Action::Press && !view.selectBlocked)
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

        if (!contacts.empty())
        {
            Node* node = contacts.front().node;

            if (dynamic_cast<Manipulator*>(node) != nullptr)
                return;

            if (mods != Modifier::Shift)
            {
                if (node)
                    view.setSelected(node);
            }
            else
            {
                while (node->getParent())
                    node = node->getParent();
            }

            if (node)
                view.setSelected(node);
        }
        else
            view.setSelected(nullptr);
    }
}

void SelectOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}
