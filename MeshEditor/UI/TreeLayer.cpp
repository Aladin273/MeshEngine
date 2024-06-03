#pragma once

#include "TreeLayer.h"
#include "../Application/View.h"

TreeLayer::TreeLayer(View* view) : BaseLayer(view)
{

}

void TreeLayer::render()
{
    ImGui::ShowAboutWindow();
}
