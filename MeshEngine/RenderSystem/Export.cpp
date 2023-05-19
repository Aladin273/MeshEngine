#include "Export.h"

#include "GLRenderSystem/GLRenderSystem.h"
#include "GLRenderSystem/GLGuiSystem.h"
#include "GLRenderSystem/GLWindow.h"

#ifdef OGL_RENDER_SYSTEM_API_DLL

__declspec(dllimport) IRenderSystem* createRenderSystem()
{
    return new GLRenderSystem();
}

__declspec(dllimport) IGuiSystem* createGuiSystem(IWindow* window)
{
    return new GLGuiSystem(window);
}

__declspec(dllimport) IWindow* createWindow(const std::string& title, uint32_t width, uint32_t height)
{
    return new GLWindow(title, width, height);
}

__declspec(dllimport) void waitEvents()
{
    glfwWaitEvents();
}

__declspec(dllimport) void pollEvents()
{
    glfwPollEvents();
}

__declspec(dllimport) void swapDisplayBuffers(IWindow* window)
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

__declspec(dllimport) bool windowShouldClose(IWindow* window)
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

#endif

#ifdef OGL_RENDER_SYSTEM_API_STATIC

IRenderSystem* MeshEngine::createRenderSystem()
{
    return new GLRenderSystem();
}

IGuiSystem* MeshEngine::createGuiSystem(IWindow* window)
{
    return new GLGuiSystem(window);
}

IWindow* MeshEngine::createWindow(const std::string& title, uint32_t width, uint32_t height)
{
    return new GLWindow(title, width, height);
}

void MeshEngine::waitEvents()
{
    glfwWaitEvents();
}

void MeshEngine::pollEvents()
{
    glfwPollEvents();
}

void MeshEngine::swapDisplayBuffers(IWindow* window)
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

bool MeshEngine::windowShouldClose(IWindow* window)
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

#endif
