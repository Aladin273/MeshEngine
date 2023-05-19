#pragma once

#include <stdexcept>
#include <vector>
#include <stack>
#include <map>

#include "Operator.h"

class OperatorDispatcher
{
public:
    friend class View;

    void addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op);
    void addOperator(ButtonCode button, std::unique_ptr<Operator> op);
    void addOperator(KeyCode key, std::unique_ptr<Operator> op);

    template<class Lambda>
    void addOperator(KeyCode key, Lambda lambda)
    {
        class QuickOperator : public Operator
        {
            Lambda& m_lambda;

        public:
            QuickOperator(Lambda& lambda) : m_lambda(lambda) {}

            void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override
            {
                m_lambda(view, action, mods);
            }
        };

        m_keys.emplace(key, new QuickOperator(lambda));
    }

private:
    void processMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y);
    void processMouseMove(View& view, double x, double y);
    void processKeyboardInput(View& view, KeyCode key, Action action, Modifier mods);

    std::map<ButtonCode, std::unique_ptr<Operator>> m_buttons;
    std::map<KeyCode, std::unique_ptr<Operator>> m_keys;
    std::map<KeyCode, std::pair<KeyCode, std::unique_ptr<Operator>>> m_dominants;

    std::stack<KeyCode> m_stack;
    Operator* op = nullptr;
};
