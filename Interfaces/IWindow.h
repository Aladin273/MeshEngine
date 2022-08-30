#pragma once

#include <functional>
#include <string>

enum class Modifier
{
    NoModifier = 0,
    Shift = 1,
    Control = 2,
    Alt = 4,
    Super = 8,
};

enum class Action
{
    Release = 0,
    Press = 1,
    Repeat = 2,
};

enum class ButtonCode
{
    Button_1 = 0,
    Button_2 = 1,
    Button_3 = 2,
    Button_4 = 3,
    Button_5 = 4,
    Button_6 = 5,
    Button_7 = 6,
    Button_8 = 7,
    Button_Last = Button_8,
    Button_Left = Button_1,
    Button_Right = Button_2,
    Button_Middle = Button_3,
};

enum class KeyCode
{
    //.... repeats all key codes from the glfw header

    Unknowm = -1,
    Space = 32,
    Apostrophe = 39,
    Comma = 44,
    Minus = 45,
    Period = 46,
    Slash = 47,
    Key_0 = 48,
    Key_1 = 49,
    Key_2 = 50,
    Key_3 = 51,
    Key_4 = 52,
    Key_5 = 53,
    Key_6 = 54,
    Key_7 = 55,
    Key_8 = 56,
    Key_9 = 57,
    Semicolon = 59,
    Equal = 61,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    Left_Bracket = 91,  /* [ */
    Backslash = 92,  /* \ */
    Right_Bracket = 93,  /* ] */
    Grave_Accent = 96,  /* ` */
    World_1 = 161, /* non-US #1 */
    World_2 = 162, /* non-US #2 */

    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Inset = 260,
    Delete = 261,
    Rigth = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    Page_Up = 266,
    Page_Down = 267,
    Home = 268,
    End = 269,
    Caps_Lock = 280,
    Scroll_Lock = 281,
    Num_Lock = 282,
    Print_Screen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    KP_0 = 320,
    KP_1 = 321,
    KP_2 = 322,
    KP_3 = 323,
    KP_4 = 324,
    KP_5 = 325,
    KP_6 = 326,
    KP_7 = 327,
    KP_8 = 328,
    KP_9 = 329,
    KP_Decimal = 330,
    KP_Divide = 331,
    KP_Multiply = 332,
    KP_Subtract = 333,
    KP_Add = 334,
    KP_Enter = 335,
    KP_Equal = 336,
    LEFT_Shift = 340,
    LEFT_Control = 341,
    LEFT_Alt = 342,
    LEFT_Super = 343,
    Right_Shift = 344,
    Right_Control = 345,
    Right_Alt = 346,
    Right_Super = 347,
    Menu = 348
};

class IWindow
{
public:
    using KeyCallback = std::function<void(KeyCode, Action, Modifier)>;
    using CursorPosCallback = std::function<void(double, double)>;
    using MouseCallback = std::function<void(ButtonCode, Action, Modifier, double, double)>;
    using ScrollCallback = std::function<void(double, double)>;
    using FramebufferSizeCallback = std::function<void(double, double)>;

    virtual ~IWindow() {}
    virtual uint32_t getWidth() const = 0;
    virtual uint32_t getHeight() const = 0;
    virtual const std::string& getTitle() const = 0;

    virtual Action getKey(KeyCode) const = 0;
    virtual Action getMouseButton(ButtonCode code) const = 0;
    virtual void getCursorPos(double& x, double& y) const = 0;

    virtual void* getHandle() const = 0;
    virtual void* getCurrentContext() const = 0;

    virtual void makeContextCurrent() const = 0;

    virtual void setCurrentContext(void*) = 0;
    virtual void setKeyCallback(const KeyCallback& callback) = 0;
    virtual void setCursorPosCallback(const CursorPosCallback& callback) = 0;
    virtual void setMouseCallback(const MouseCallback& callback) = 0;
    virtual void setScrollCallback(const ScrollCallback& callback) = 0;
    virtual void setFramebufferSizeCallback(const FramebufferSizeCallback& callback) = 0;
};


