#pragma once

#include <map>

//#include <glfw/glfw3.h>
//#include <imgui/imgui.h>
//#include <imgui/backends/imgui_impl_glfw.h>
//#include <imgui/backends/imgui_impl_opengl2.h>

#include "../IWindow.h"
#include "../IGuiSystem.h"

class GLGuiSystem : public IGuiSystem
{
public:
    GLGuiSystem(IWindow* window);
    virtual ~GLGuiSystem();

    void init() override;

    void begin() override;
    void end() override;
    void render() override;
    void render(IGuiLayer&) override;

    void setContext(void*) override;
    void* getContext() override;

    void pushLayer(uint32_t index, IGuiLayer& layer) override;
    void popLayer(uint32_t index) override;

    void turnDocking(bool enable) override;
    void turnVieports(bool enable) override;

    bool wantCapture() override;

private:
    IWindow* m_window;
    std::map<uint32_t, IGuiLayer*> m_layers;
};

