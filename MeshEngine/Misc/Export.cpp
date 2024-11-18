#include "Export.h"

#ifdef OGL_RENDER_SYSTEM_API_DLL

#include "MeshEngine/RenderSystem/OpenGL/GLRenderSystem.h"
#include "MeshEngine/RenderSystem/OpenGL/GLGuiSystem.h"
#include "MeshEngine/RenderSystem/OpenGL/GLShader.h"
#include "MeshEngine/RenderSystem/OpenGL/GLWindow.h"

#include <glad/gl.h>
#include <glfw/glfw3.h>

__declspec(dllimport) RenderSystem* createRenderSystem()
{
    return new GLRenderSystem();
}

__declspec(dllimport) GuiSystem* createGuiSystem(Window* window)
{
    return new GLGuiSystem(window);
}

__declspec(dllimport) Shader* createShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
    return new GLShader(vertexPath, fragmentPath, geometryPath);
}

__declspec(dllimport) Window* createWindow(const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    return new GLWindow(title, width, height, icon);
}

__declspec(dllimport) void waitEvents()
{
    glfwWaitEvents();
}

__declspec(dllimport) void pollEvents()
{
    glfwPollEvents();
}

__declspec(dllimport) void swapDisplayBuffers(Window* window)
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

__declspec(dllimport) bool windowShouldClose(Window* window)
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

#endif

#ifdef OGL_RENDER_SYSTEM_API_STATIC

#include "MeshEngine/RenderSystem/OpenGL/GLRenderSystem.h"
#include "MeshEngine/RenderSystem/OpenGL/GLGuiSystem.h"
#include "MeshEngine/RenderSystem/OpenGL/GLShader.h"
#include "MeshEngine/RenderSystem/OpenGL/GLWindow.h"

#include <glad/gl.h>
#include <glfw/glfw3.h>

RenderSystem* MeshEngine::createRenderSystem()
{
    return new GLRenderSystem();
}

GuiSystem* MeshEngine::createGuiSystem(Window* window)
{
    return new GLGuiSystem(window);
}

Shader* MeshEngine::createShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
    return new GLShader(vertexPath, fragmentPath, geometryPath);
}

Window* MeshEngine::createWindow(const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    return new GLWindow(title, width, height, icon);
}

void MeshEngine::waitEvents()
{
    glfwWaitEvents();
}

void MeshEngine::pollEvents()
{
    glfwPollEvents();
}

void MeshEngine::swapDisplayBuffers(Window* window)
{
    glfwSwapBuffers(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

bool MeshEngine::windowShouldClose(Window* window)
{
    return glfwWindowShouldClose(reinterpret_cast<GLFWwindow*>(window->getHandle()));
}

#endif
