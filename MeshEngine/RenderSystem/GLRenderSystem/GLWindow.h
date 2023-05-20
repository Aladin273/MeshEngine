#pragma once

#include "ThirdParty/glad/glad.h"
#include "ThirdParty/glfw/glfw3.h"

#include "RenderSystem/IWindow.h"

class GLWindow : public IWindow
{
public:
    GLWindow(const std::string& title, uint32_t width, uint32_t height);
    virtual ~GLWindow();

    uint32_t getWidth() const override;
    uint32_t getHeight() const override;
    const std::string& getTitle() const override;

    Action getKey(KeyCode) const override;
    Action getMouseButton(ButtonCode code) const override;
    void getCursorPos(double& x, double& y) const override;

    void* getHandle() const override;
    void* getCurrentContext() const override;

    void setCurrentContext() const override;
    void setKeyCallback(const KeyCallback& callback) override;
    void setCursorPosCallback(const CursorPosCallback& callback) override;
    void setMouseCallback(const MouseCallback& callback) override;
    void setScrollCallback(const ScrollCallback& callback) override;
    void setFramebufferSizeCallback(const FramebufferSizeCallback& callback) override;

    friend void onKeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void onCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
    friend void onMouseCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
    friend void onScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
    friend void onFramebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);

private:
    GLFWwindow* m_handle = nullptr;

    std::string m_title = "GLWindow";
    
    uint32_t m_width = 800;
    uint32_t m_height = 600;

    std::vector<KeyCallback> m_keyCallbacks;
    std::vector<CursorPosCallback> m_cursorCallbacks;
    std::vector<MouseCallback> m_mouseCallbacks;
    std::vector<ScrollCallback> m_scrollCallbacks;
    std::vector<FramebufferSizeCallback> m_sizeCallbacks;
};

void onKeyCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
void onCursorCallbackWrapper(GLFWwindow* window, double xpos, double ypos);
void onMouseCallbackWrapper(GLFWwindow* window, int button, int action, int mods);
void onScrollCallbackWrapper(GLFWwindow* window, double xoffset, double yoffset);
void onFramebufferSizeCallbackWrapper(GLFWwindow* window, int width, int height);
