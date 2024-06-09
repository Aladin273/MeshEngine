#pragma once

#include <string>

#include "MeshEngine/RenderSystem/GuiSystem.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"
#include "MeshEngine/RenderSystem/Shader.h"
#include "MeshEngine/RenderSystem/Window.h"

#ifndef OGL_RENDER_SYSTEM_EXPORT_DLL
#define OGL_RENDER_SYSTEM_EXPORT_STATIC
#endif

#ifdef OGL_RENDER_SYSTEM_EXPORT_DLL
#define OGL_RENDER_SYSTEM_API_DLL
#endif

#ifdef OGL_RENDER_SYSTEM_EXPORT_STATIC
#define OGL_RENDER_SYSTEM_API_STATIC
#endif

namespace MeshEngine
{
    #ifdef OGL_RENDER_SYSTEM_API_DLL

        extern "C" __declspec(dllimport) GuiSystem* createGuiSystem(Window* window);
        extern "C" __declspec(dllimport) RenderSystem* createRenderSystem();
        extern "C" __declspec(dllimport) Shader* createShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
        extern "C" __declspec(dllimport) Window* createWindow(const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");
        extern "C" __declspec(dllimport) Window* createWindow(const std::string& title, uint32_t width, uint32_t height);
        extern "C" __declspec(dllimport) void waitEvents();
        extern "C" __declspec(dllimport) void pollEvents();
        extern "C" __declspec(dllimport) void swapDisplayBuffers(Window* window);
        extern "C" __declspec(dllimport) bool windowShouldClose(Window* window);

    #endif
    
    #ifdef OGL_RENDER_SYSTEM_API_STATIC

        GuiSystem* createGuiSystem(Window* window);
        RenderSystem* createRenderSystem();
        Shader* createShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath = "");
        Window* createWindow(const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");
        void waitEvents();
        void pollEvents();
        void swapDisplayBuffers(Window* window);
        bool windowShouldClose(Window* window);

    #endif
}
