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
        ImGui::Spacing();

        glm::vec3 translation, scale, skew;
        glm::vec4 perspective;
        glm::quat rotation;

        glm::decompose(m_view->getSelected()->getRelativeTransform(), scale, rotation, translation, skew, perspective);
        glm::vec3 rotationEuler = glm::eulerAngles(rotation);

        // Translation
        ImGui::Text("Translation");
        ImGui::Spacing();
        ImGui::DragFloat3("##Translation", glm::value_ptr(translation), 0.1f);
        ImGui::Spacing();

        // Rotation
        ImGui::Text("Rotation");
        ImGui::Spacing();
        glm::vec3 rotationDegrees = glm::degrees(rotationEuler);

        if (ImGui::DragFloat3("##Rotation", glm::value_ptr(rotationDegrees), 0.1f))
        {
            rotationEuler = glm::radians(rotationDegrees);
            rotation = glm::quat(rotationEuler);
        }
        ImGui::Spacing();

        // Scale
        ImGui::Text("Scale");
        ImGui::Spacing();
        ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.01f);
        ImGui::Spacing();

        // Recompose the matrix
        if (scale.x != 0.f && scale.y != 0.f && scale.z != 0.f)
            m_view->getSelected()->setRelativeTransform(glm::translate(glm::mat4(1.0f), translation) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale));

        // Mesh
        ImGui::Separator();
        ImGui::Text(m_view->getSelected()->getMesh()->getName().c_str());
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::Checkbox("Render Triangles", &(m_view->getSelected()->getMesh()->renderTriangles));
        ImGui::Spacing();
        ImGui::Checkbox("Render Lines", &(m_view->getSelected()->getMesh()->renderLines));
        ImGui::Spacing();
        ImGui::Checkbox("Render Holes", &(m_view->getSelected()->getMesh()->renderHoles));
        ImGui::Spacing();
        ImGui::Checkbox("Render Boundaries", &(m_view->getSelected()->getMesh()->renderBoundaries));
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Material");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::ColorEdit3("Ambient", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().ambient));
        ImGui::Spacing();
        ImGui::ColorEdit4("Diffuse", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().diffuse));
        ImGui::Spacing();
        ImGui::ColorEdit3("Specular", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().specular));
        ImGui::Spacing();
        ImGui::ColorEdit3("Emission", glm::value_ptr(m_view->getSelected()->getMesh()->getMaterial().emission));
        ImGui::Spacing();
        ImGui::SliderFloat("Shininess", &m_view->getSelected()->getMesh()->getMaterial().shininess, 1.0f, 256.0f);
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Colors");
        ImGui::Separator();
        ImGui::Spacing();

        ///ImGui::ColorEdit4("Color Triangles", glm::value_ptr(m_view->getSelected()->getMesh()->colorTriangles));
        //ImGui::Spacing();
        ImGui::ColorEdit4("Color Lines", glm::value_ptr(m_view->getSelected()->getMesh()->colorLines));
        ImGui::Spacing();
        ImGui::ColorEdit4("Color Holes", glm::value_ptr(m_view->getSelected()->getMesh()->colorHoles));
        ImGui::Spacing();
        ImGui::ColorEdit4("Color Boundaries", glm::value_ptr(m_view->getSelected()->getMesh()->colorBoundaries));
        ImGui::Spacing();
    }
    
    ImGui::End();
}
