#include "GLWindow.h"

GLWindow::GLWindow(const std::string& title, uint32_t width, uint32_t height)
    : m_title(title), m_width(width), m_height(height)
{
    glfwInit();

    m_handle = glfwCreateWindow(m_width, m_height, m_title.data(), nullptr, glfwGetCurrentContext());
    glfwMakeContextCurrent(m_handle);
    glfwSwapInterval(0);

    static bool initGLAD = false;
    if (!initGLAD)
    {
        initGLAD = true;
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    }

    glfwSetWindowUserPointer(m_handle, this);
    glfwSetInputMode(m_handle, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    glfwSetInputMode(m_handle, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(m_handle, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    glfwSetFramebufferSizeCallback(m_handle, onFramebufferSizeCallbackWrapper);
}

GLWindow::~GLWindow()
{
    glfwDestroyWindow(m_handle);
}

uint32_t GLWindow::getWidth() const
{
    return m_width;
}

uint32_t GLWindow::getHeight() const
{
    return m_height;
}

const std::string& GLWindow::getTitle() const
{
    return m_title;
}

Action GLWindow::getKey(KeyCode code) const
{
    return static_cast<Action>(glfwGetKey(m_handle, static_cast<int>(code)));
}

Action GLWindow::getMouseButton(ButtonCode code) const
{
    return static_cast<Action>(glfwGetMouseButton(m_handle, static_cast<int>(code)));
}

void GLWindow::getCursorPos(double& x, double& y) const
{
    glfwGetCursorPos(m_handle, &x, &y);
    y = m_height - y - 1.0f;
}

void* GLWindow::getHandle() const
{
    return m_handle;
}

void* GLWindow::getCurrentContext() const
{
    return glfwGetCurrentContext();
}

void GLWindow::makeContextCurrent() const
{
    glfwMakeContextCurrent(m_handle);
}

void GLWindow::setCurrentContext(void* context)
{
    glfwMakeContextCurrent(reinterpret_cast<GLFWwindow*>(context));
}

void GLWindow::setKeyCallback(const KeyCallback& callback)
{
    m_keyCallback = callback;
    glfwSetKeyCallback(m_handle, onKeyCallbackWrapper);
}

void GLWindow::setCursorPosCallback(const CursorPosCallback& callback)
{
    m_cursorCallback = callback;
    glfwSetCursorPosCallback(m_handle, onCursorCallbackWrapper);
}

void GLWindow::setMouseCallback(const MouseCallback& callback)
{
    m_mouseCallback = callback;
    glfwSetMouseButtonCallback(m_handle, onMouseCallbackWrapper);
}

void GLWindow::setScrollCallback(const ScrollCallback& callback)
{
    m_scrollCallback = callback;
    glfwSetScrollCallback(m_handle, onScrollCallbackWrapper);
}

void GLWindow::setFramebufferSizeCallback(const FramebufferSizeCallback& callback)
{
    m_sizeCallback = callback;
    glfwSetFramebufferSizeCallback(m_handle, onFramebufferSizeCallbackWrapper);
}

void onKeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    GLWindow* current = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    current->m_keyCallback(static_cast<KeyCode>(key), static_cast<Action>(action), static_cast<Modifier>(mods));
}

void onCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos)
{
    GLWindow* current = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    current->m_cursorCallback(xpos, current->m_height - ypos - 1.0f);
}

void onMouseCallbackWrapper(GLFWwindow* window, int button, int action, int mods)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    GLWindow* current = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    current->m_mouseCallback(static_cast<ButtonCode>(button), static_cast<Action>(action), static_cast<Modifier>(mods), xpos, current->m_height - ypos - 1.0f);
}

void onScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset)
{
    GLWindow* current = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    current->m_scrollCallback(xoffset, yoffset);
}

void onFramebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height)
{
    GLWindow* current = reinterpret_cast<GLWindow*>(glfwGetWindowUserPointer(window));
    current->m_width = width;
    current->m_height = height;

    if (current->m_sizeCallback != nullptr)
        current->m_sizeCallback(width, height);
}
