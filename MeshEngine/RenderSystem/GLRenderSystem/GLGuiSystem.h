#pragma once

#include <map>

//#include "ThirdParty/glfw/glfw3.h"
//#include "ThirdParty/imgui/imgui.h"
//#include "ThirdParty/imgui/backends/imgui_impl_glfw.h"
//#include "ThirdParty/imgui/backends/imgui_impl_opengl2.h"

#include "RenderSystem/IWindow.h"
#include "RenderSystem/IGuiSystem.h"

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

