#pragma once

#include "MeshEngine/RenderSystem/GuiSystem.h"

#include <imgui.h>

#include <glm/glm.hpp>

class View;

class BaseLayer : public GuiLayer
{
public:
    BaseLayer(View* view);

    virtual void render() override;

protected:
    View* m_view;
};