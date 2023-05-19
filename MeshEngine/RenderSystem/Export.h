#pragma once

#include <string>

#include "IWindow.h"
#include "IGuiSystem.h"
#include "IRenderSystem.h"

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

        extern "C" __declspec(dllimport) IRenderSystem * createRenderSystem();
        extern "C" __declspec(dllimport) IGuiSystem * createGuiSystem(IWindow * window);
        extern "C" __declspec(dllimport) IWindow * createWindow(const std::string & title, uint32_t width, uint32_t height);
        extern "C" __declspec(dllimport) void waitEvents();
        extern "C" __declspec(dllimport) void pollEvents();
        extern "C" __declspec(dllimport) void swapDisplayBuffers(IWindow * window);
        extern "C" __declspec(dllimport) bool windowShouldClose(IWindow * window);

    #endif
    
    #ifdef OGL_RENDER_SYSTEM_API_STATIC

        IRenderSystem* createRenderSystem();
        IGuiSystem* createGuiSystem(IWindow* window);
        IWindow* createWindow(const std::string& title, uint32_t width, uint32_t height);
        void waitEvents();
        void pollEvents();
        void swapDisplayBuffers(IWindow* window);
        bool windowShouldClose(IWindow* window);

    #endif
}
