#pragma once

#include "PropertiesLayer.h"
#include "MeshEditor/Editor/View.h"

PropertiesLayer::PropertiesLayer(View* view) : BaseLayer(view)
{

}

void PropertiesLayer::render()
{
    ImGui::Begin("Properties");

    if (m_view->getSelected().node)
    {
        renderProperties(m_view->getSelected().node, m_view->getSelected().node->getName(), 0);
    }

    ImGui::End();
}

void PropertiesLayer::renderProperties(Base* base, const std::string& name, uint32_t level)
{
    if (base)
    {
        if (ImGui::TreeNodeEx(base, level < m_levelOpened ? ImGuiTreeNodeFlags_DefaultOpen : ImGuiTreeNodeFlags_None, "%s", name.c_str()))
        {
            ImGui::Separator();

            for (const Property& property : base->getProperties())
            {
                ImGui::Spacing();

                switch (property.type)
                {
                case Property::Int:
                {
                    if (ImGui::DragInt(property.name.c_str(), (int*)property.object))
                        base->propertyChanged(property);
                    
                    break;
                };

                case Property::Float:
                {
                    if (ImGui::DragFloat(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Double:
                {
                    if (ImGui::DragFloat(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Bool:
                {
                    if (ImGui::Checkbox(property.name.c_str(), (bool*)property.object))
                        base->propertyChanged(property);

                    break;
                };

                case Property::String:
                {
                    const std::string& text = *((std::string*)property.object);

                    ImGui::Text((property.name + std::string(": ") + text).c_str());
                    break;
                };

                case Property::Vector2:
                {
                    if (ImGui::DragFloat2(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };
                case Property::Vector3:
                {
                    if (ImGui::DragFloat3(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Vector4:
                {
                    if (ImGui::DragFloat4(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Matrix2:
                {
                    ImGui::Text(property.name.c_str());

                    if (ImGui::DragFloat2("", &((float*)property.object)[0], 0.01f) ||
                        ImGui::DragFloat2("", &((float*)property.object)[2], 0.01f))
                    {
                        base->propertyChanged(property);
                    }

                    break;
                };

                case Property::Matrix3:
                {
                    ImGui::Text(property.name.c_str());
                    
                    if (ImGui::DragFloat3("", &((float*)property.object)[0], 0.01f) ||
                        ImGui::DragFloat3("", &((float*)property.object)[3], 0.01f) ||
                        ImGui::DragFloat3("", &((float*)property.object)[6], 0.01f))
                    {
                        base->propertyChanged(property);
                    }

                    break;
                };

                case Property::Matrix4:
                {
                    ImGui::Text(property.name.c_str());
                    
                    if (ImGui::DragFloat4("", &((float*)property.object)[0], 0.01f) ||
                        ImGui::DragFloat4("", &((float*)property.object)[4], 0.01f) ||
                        ImGui::DragFloat4("", &((float*)property.object)[8], 0.01f) ||
                        ImGui::DragFloat4("", &((float*)property.object)[12], 0.01f))
                    {
                        base->propertyChanged(property);
                    }

                    break;
                };

                case Property::MatrixEx:
                {
                    ImGui::Text(property.name.c_str());

                    glm::vec3 translation, rotation, scale;
                    ImGuizmo::DecomposeMatrixToComponents((float*)property.object, glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale));

                    bool propertyChaged = false;

                    ImGui::Spacing();
                    propertyChaged |= ImGui::DragFloat3("Translation", glm::value_ptr(translation), 1.f);
                    ImGui::Spacing();
                    propertyChaged |= ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 1.f);
                    ImGui::Spacing();
                    propertyChaged |= ImGui::DragFloat3("Scale", glm::value_ptr(scale), 1.f);
                    
                    if (propertyChaged)
                    {
                        ImGuizmo::RecomposeMatrixFromComponents(glm::value_ptr(translation), glm::value_ptr(rotation), glm::value_ptr(scale), (float*)property.object);
                        base->propertyChanged(property);
                        
                        m_view->setSelected(m_view->getSelected()); // Temp
                    }

                    break;
                };

                case Property::Quat:
                {
                    if (ImGui::DragFloat4(property.name.c_str(), (float*)property.object, 0.01f))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Color:
                {
                    if (ImGui::ColorEdit3(property.name.c_str(), (float*)property.object))
                        base->propertyChanged(property);

                    break;
                };

                case Property::ColorEx:
                {
                    if (ImGui::ColorEdit4(property.name.c_str(), (float*)property.object))
                        base->propertyChanged(property);

                    break;
                };

                case Property::Separator:
                {
                    ImGui::Separator();
                    break;
                };

                case Property::Base:
                {
                    renderProperties((Base*)property.object, property.name, level + 1);
                    break;
                }
                };
            }

            ImGui::TreePop();
        }
    }
}
