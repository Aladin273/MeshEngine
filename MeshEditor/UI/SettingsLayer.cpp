#pragma once

#include "PropertiesLayer.h"
#include "MeshEditor/Editor/View.h"


SettingsLayer::SettingsLayer(View* view) : PropertiesLayer(view)
{

}

void SettingsLayer::render()
{
    ImGui::Begin("Settings");
    
    renderProperties(m_view, "View");
    renderProperties(m_view->getScene(), "Scene");

    ImGui::End();
}
