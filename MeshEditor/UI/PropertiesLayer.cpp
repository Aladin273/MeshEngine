#pragma once

#include "PropertiesLayer.h"
#include "../Application/View.h"

PropertiesLayer::PropertiesLayer(View* view) : BaseLayer(view)
{

}

void PropertiesLayer::render()
{
    ImGui::ShowDemoWindow();
}
