#pragma once

#include "ConsoleLayer.h"
#include "../Application/View.h"

ConsoleLayer::ConsoleLayer(View* view) : BaseLayer(view)
{

}

void ConsoleLayer::render()
{
    ImGui::ShowDebugLogWindow();
}
