#pragma once

#include <map>

//#include "ThirdParty/glfw/glfw3.h"
//#include "ThirdParty/imgui/imgui.h"
//#include "ThirdParty/imgui/backends/imgui_impl_glfw.h"
//#include "ThirdParty/imgui/backends/imgui_impl_opengl2.h"

#include <MeshEngine/RenderSystem/Window.h>
#include <MeshEngine/RenderSystem/GuiSystem.h>

class GLGuiSystem : public GuiSystem
{
public:
    GLGuiSystem(Window* window);
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
    Window* m_window;
    std::map<uint32_t, IGuiLayer*> m_layers;
};