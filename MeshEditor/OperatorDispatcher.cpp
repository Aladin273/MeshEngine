#include "View.h"
#include "OperatorDispatcher.h"

void OperatorDispatcher::addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op)
{
    if (m_keys.find(enterKey) != m_keys.end())
        throw std::logic_error("An operator is a bind with a key that another operator already uses");
    if (m_dominants.find(enterKey) != m_dominants.end())
        throw std::logic_error("An operator is a bind with a key that another operator already uses");
    ;
    m_dominants.emplace(enterKey, std::make_pair(exitKey, std::move(op)));
}

void OperatorDispatcher::addOperator(ButtonCode button, std::unique_ptr<Operator> op)
{
    if (m_buttons.find(button) != m_buttons.end())
        throw std::logic_error("An operator is a bind with a button that another operator already uses");

    m_buttons.emplace(button, std::move(op));
}

void OperatorDispatcher::addOperator(KeyCode key, std::unique_ptr<Operator> op)
{
    if (m_keys.find(key) != m_keys.end())
        throw std::logic_error("An operator is a bind with a key that another operator already uses");

    m_keys.emplace(key, std::move(op));
}

void OperatorDispatcher::processMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (!m_stack.empty())
    {
        auto dominant = m_dominants.find(m_stack.top());

        if (dominant != m_dominants.end())
            op = dominant->second.second.get();
    }
    else
    {
        auto buttoncode = m_buttons.find(button);

        if (buttoncode != m_buttons.end())
            op = buttoncode->second.get();
    }

    op->onMouseInput(view, button, action, mods, x, y);
}

void OperatorDispatcher::processKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (!m_stack.empty())
    {
        auto dominant = m_dominants.find(m_stack.top());

        if (dominant != m_dominants.end())
        {
            op = dominant->second.second.get();
            op->onKeyboardInput(view, key, action, mods);

            if (key == dominant->second.first)
            {
                op->onExit(view);
                m_stack.pop();
            }
        }
    }
    else
    {
        auto keycode = m_keys.find(key);

        if (keycode != m_keys.end())
        {
            op = keycode->second.get();
            op->onKeyboardInput(view, key, action, mods);
        }
        else
        {
            auto dominant = m_dominants.find(key);

            if (dominant != m_dominants.end())
            {
                m_stack.push(key);

                op = dominant->second.second.get();
                op->onEnter(view);
                op->onKeyboardInput(view, key, action, mods);
            }
        }
    }
}

void OperatorDispatcher::processMouseMove(View& view, double x, double y)
{
    if (!m_stack.empty())
    {
        auto dominant = m_dominants.find(m_stack.top());

        if (dominant != m_dominants.end())
        {
            op = dominant->second.second.get();
            op->onMouseMove(view, x, y);
        }
    }
    else
    {
        for (auto& op : m_buttons)
            op.second->onMouseMove(view, x, y);
    }
}
