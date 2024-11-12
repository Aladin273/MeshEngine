#pragma once

#include "PropertiesLayer.h"
#include "../Application/View.h"


SettingsLayer::SettingsLayer(View* view) : PropertiesLayer(view)
{

}

void SettingsLayer::render()
{
    ImGui::Begin("Settings");
    
    processProperties(m_view, "View");
    processProperties(m_view->getScene(), "Scene");

    ImGui::End();
}
