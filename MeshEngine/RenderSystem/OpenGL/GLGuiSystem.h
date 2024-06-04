#pragma once

#include <map>

#include "MeshEngine/RenderSystem/Window.h"
#include "MeshEngine/RenderSystem/GuiSystem.h"

class GLGuiSystem : public GuiSystem
{
public:
    GLGuiSystem(Window* window);
    virtual ~GLGuiSystem();

    virtual void init() override;

    virtual void begin() override;
    virtual void end() override;

    virtual void render() override;
    virtual void render(GuiLayer&) override;

    virtual void pushLayer(uint32_t index, GuiLayer& layer) override;
    virtual void popLayer(uint32_t index) override;

    virtual bool wantCaptureMouse() override;
    virtual bool wantCaptureKeyboard() override;

private:
    Window* m_window;
    std::map<uint32_t, GuiLayer*> m_layers;
};