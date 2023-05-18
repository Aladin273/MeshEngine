#pragma once

#include <string>

#include "IWindow.h"
#include "IGuiSystem.h"
#include "IRenderSystem.h"

#ifdef OGL_RENDER_SYSTEM_EXPORT
#define OGL_RENDER_SYSTEM_API __declspec(dllexport)
#else
#define OGL_RENDER_SYSTEM_API __declspec(dllimport)
#endif

extern "C" OGL_RENDER_SYSTEM_API IRenderSystem* createRenderSystem();
extern "C" OGL_RENDER_SYSTEM_API IGuiSystem* createGuiSystem(IWindow* window);
extern "C" OGL_RENDER_SYSTEM_API IWindow* createWindow(const std::string& title, uint32_t width, uint32_t height);
extern "C" OGL_RENDER_SYSTEM_API void waitEvents();
extern "C" OGL_RENDER_SYSTEM_API void pollEvents();
extern "C" OGL_RENDER_SYSTEM_API void swapDisplayBuffers(IWindow* window);
extern "C" OGL_RENDER_SYSTEM_API bool windowShouldClose(IWindow* window);
