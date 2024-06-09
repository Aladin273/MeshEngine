#pragma once

#include "PropertiesLayer.h"
#include "../Application/View.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

SettingsLayer::SettingsLayer(View* view) : BaseLayer(view)
{

}

void SettingsLayer::render()
{
    ImGui::Begin("Settings");
    
    ImGui::ColorEdit4("Background Color", glm::value_ptr(m_view->backgroundColor));
    ImGui::Spacing();

    ImGui::DragFloat3("##Light Direction", glm::value_ptr(m_view->lightDirection), 0.1f);
    ImGui::SameLine();
    ImGui::Text("Light Direction");
    ImGui::Spacing();
    
    ImGui::Checkbox("Camera Light", &m_view->cameraLight);
    ImGui::Spacing();

    ImGui::Checkbox("Flat Shading", &m_view->flatShading);
    ImGui::Spacing();

    ImGui::Checkbox("Cast Shadows", &m_view->castShadows);
    ImGui::Spacing();

    ImGui::Checkbox("Show Origin", &m_view->showOrigin);
    ImGui::Spacing();

    ImGui::Checkbox("Show Plane", &m_view->showPlane);
    ImGui::Spacing();

    ImGui::End();
}
