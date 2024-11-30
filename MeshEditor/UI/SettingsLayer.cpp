#pragma once

#include "PropertiesLayer.h"
#include "MeshEditor/Editor/View.h"


SettingsLayer::SettingsLayer(View* view) : PropertiesLayer(view)
{

}

void SettingsLayer::render()
{
    ImGui::Begin("Settings");
    
    m_view->bind();
    m_view->getScene()->bind();

    renderProperties(m_view, "View", 0);
    renderProperties(m_view->getScene(), "Scene", 0);

    m_view->unbind();
    m_view->getScene()->unbind();

    ImGui::End();
}
