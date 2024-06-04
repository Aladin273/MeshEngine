#pragma once

#include "PropertiesLayer.h"
#include "../Application/View.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

PropertiesLayer::PropertiesLayer(View* view) : BaseLayer(view)
{

}

void PropertiesLayer::render()
{
    ImGui::Begin("Properties");
    
    if (m_view->getSelected())
    {
        ImGui::Text(m_view->getSelected()->getName().c_str());
        ImGui::Separator();

        glm::vec3 translation, scale, skew;
        glm::vec4 perspective;
        glm::quat rotation;

        glm::decompose(m_view->getSelected()->getRelativeTransform(), scale, rotation, translation, skew, perspective);
        glm::vec3 rotationEuler = glm::eulerAngles(rotation);

        // Translation
        ImGui::Text("Translation");
        ImGui::DragFloat3("##Translation", glm::value_ptr(translation), 0.1f);

        // Rotation
        ImGui::Text("Rotation");
        glm::vec3 rotationDegrees = glm::degrees(rotationEuler);

        if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotationDegrees), 0.1f))
        {
            rotationEuler = glm::radians(rotationDegrees);
            rotation = glm::quat(rotationEuler);
        }

        // Scale
        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.1f);

        // Recompose the matrix
        m_view->getSelected()->setRelativeTransform(glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale));

        ImGui::Separator();

        // Mesh
        ImGui::Text(m_view->getSelected()->getMesh()->getName().c_str());
        ImGui::Separator();

        ImGui::Checkbox("Render Triangles", &(m_view->getSelected()->getMesh()->renderTriangles));
        ImGui::Checkbox("Render Lines", &(m_view->getSelected()->getMesh()->renderLines));
        ImGui::Checkbox("Render Holes", &(m_view->getSelected()->getMesh()->renderHoles));
        ImGui::Checkbox("Render Boundaries", &(m_view->getSelected()->getMesh()->renderBoundaries));
        
        ImGui::Separator();

        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().ambient));
        ImGui::ColorEdit4("Diffuse", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().diffuse));
        ImGui::ColorEdit3("Specular", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().specular));
        ImGui::ColorEdit3("Emission", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().emission));
        ImGui::SliderFloat("Shininess", &m_view->getSelected()->getMesh()->getMaterial().shininess, 1.0f, 256.0f);

        ImGui::Separator();

        ///ImGui::ColorEdit4("Color Triangles", glm::value_ptr(m_view->getSelected()->getMesh()->colorTriangles));
        ImGui::ColorEdit4("Color Lines", glm::value_ptr(m_view->getSelected()->getMesh()->colorLines));
        ImGui::ColorEdit4("Color Holes", glm::value_ptr(m_view->getSelected()->getMesh()->colorHoles));
        ImGui::ColorEdit4("Color Boundaries", glm::value_ptr(m_view->getSelected()->getMesh()->colorBoundaries));
    }
    
    ImGui::End();
}
