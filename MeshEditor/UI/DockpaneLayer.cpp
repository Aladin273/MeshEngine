#pragma once

#include "DockpaneLayer.h"
#include "../Application/View.h"

DockpaneLayer::DockpaneLayer(View* view) : BaseLayer(view)
{

}

void DockpaneLayer::render()
{
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}
