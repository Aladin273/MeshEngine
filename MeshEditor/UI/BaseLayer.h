#pragma once

#include "MeshEngine/RenderSystem/GuiSystem.h"

#include <string>
#include <glm/glm.hpp>

#include <imgui.h>
#include <ImGuizmo.h>

class View;

class BaseLayer : public GuiLayer
{
public:
    BaseLayer(View* view);

    virtual void render() override;

protected:
    View* m_view;
};