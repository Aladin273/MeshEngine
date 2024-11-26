#include "MeshEditor/Editor/View.h"

#include "OperatorDispatcher.h"

void OperatorDispatcher::forceOperator(View& view, KeyCode key)
{
    if (m_dominants.find(key) != m_dominants.end())
    {
        m_stack.push(key);
    }
    else if (m_keys.find(key) != m_keys.end())
    {
        m_op = m_keys[key].get();
        m_op->onEnter(view);
    }
}

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
    //if (m_buttons.find(button) != m_buttons.end())
    //    throw std::logic_error("An operator is a bind with a button that another operator already uses");

    m_buttons.push_back({ button, std::move(op) });
}

void OperatorDispatcher::addOperator(KeyCode key, std::unique_ptr<Operator> op)
{
    if (m_keys.find(key) != m_keys.end())
        throw std::logic_error("An operator is a bind with a key that another operator already uses");

    m_keys.emplace(key, std::move(op));
}

void OperatorDispatcher::processMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (m_stack.size())
    {
        auto keycode = m_dominants.find(m_stack.top());

        if (keycode != m_dominants.end())
        {
            keycode->second.second.get()->onMouseInput(view, button, action, mods, x, y);
        }
    }
    else if (m_op)
    {
        m_op->onMouseInput(view, button, action, mods, x, y);
    }

    for (auto& op : m_buttons)
    {
        if (button == op.first)
            op.second->onMouseInput(view, button, action, mods, x, y);
    }
}

void OperatorDispatcher::processKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
    if (m_stack.size())
    {
        auto keycode = m_dominants.find(m_stack.top());

        if (keycode != m_dominants.end())
        {
            if (key == keycode->second.first)
            {
                keycode->second.second.get()->onExit(view);
                m_stack.pop();
            }
            else
            {
                keycode->second.second.get()->onKeyboardInput(view, key, action, mods);
            }
        }
    }
    else
    {
        auto keycode = m_keys.find(key);

        if (keycode != m_keys.end())
        {
            Operator* op = keycode->second.get();

            if (m_op && m_op != op)
                m_op->onExit(view);

            m_op = op;
            m_op->onEnter(view);
        }
        else
        {
            auto keycode = m_quicks.find(key);

            if (keycode != m_quicks.end())
            {
                keycode->second.get()->onKeyboardInput(view, key, action, mods);
            }
            else
            {
                if (m_stack.size()) 
                {
                    auto keycode = m_dominants.find(key);

                    if (keycode != m_dominants.end())
                    {
                        m_stack.push(key);
                        keycode->second.second.get()->onEnter(view);
                    }

                }
                else if (m_op)
                {
                    m_op->onKeyboardInput(view, key, action, mods);
                }
            }
        }
    }
}

void OperatorDispatcher::processMouseMove(View& view, double x, double y)
{
    if (m_stack.size())
    {
        auto keycode = m_dominants.find(m_stack.top());

        if (keycode != m_dominants.end())
        {
            keycode->second.second.get()->onMouseMove(view, x, y);
        }
    }
    else
    {
        for (auto& op : m_keys)
            op.second->onMouseMove(view, x, y);
    }

    for (auto& op : m_buttons)
        op.second->onMouseMove(view, x, y);
}
