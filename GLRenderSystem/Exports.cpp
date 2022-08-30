#include "Exports.h"

OGL_RENDER_SYSTEM_API IRenderSystem* createRenderSystem()
{
    return new GLRenderSystem();
}

OGL_RENDER_SYSTEM_API IGuiSystem* createGuiSystem(IWindow* window)
{
    return new GLGuiSystem(window);
}

OGL_RENDER_SYSTEM_API IWindow* createWindow(const std::string& title, uint32_t width, uint32_t height)
{
    return new GLWindow(title, width, height);
}

OGL_RENDER_SYSTEM_API void waitEvents()
{
    glfwWaitEvents();
}

OGL_RENDER_SYSTEM_API void pollEvents()
{
    glfwPollEvents();
}

OGL_RENDER_SYSTEM_API void swapDisplayBuffers(IWindow* window)
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

OGL_RENDER_SYSTEM_API bool windowShouldClose(IWindow* window)
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}
