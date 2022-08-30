#pragma once

class View;
enum class KeyCode;
enum class ButtonCode;
enum class Action;
enum class Modifier;

class Operator
{
public:
    virtual ~Operator() {}
    virtual void onEnter(View&) {}
    virtual void onExit(View&) {}
    virtual void onMouseMove(View& view, double x, double y) {}
    virtual void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) {}
    virtual void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) {}
};