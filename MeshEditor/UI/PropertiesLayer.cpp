#pragma once

#include "PropertiesLayer.h"
#include "MeshEditor/Editor/View.h"

PropertiesLayer::PropertiesLayer(View* view) : BaseLayer(view)
{

}

void PropertiesLayer::render()
{
    ImGui::Begin("Properties");

    if (m_view->getSelected())
    {
        processProperties(m_view->getSelected(), m_view->getSelected()->getName());
    }

    ImGui::End();
}

void PropertiesLayer::processProperties(Base* base, const std::string& name)
{
    if (base)
    {
        base->bind();

        if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::Separator();

            for (const Property& property : base->getProperties())
            {
                ImGui::Spacing();

                switch (property.type)
                {
                case Property::Int:
                {
                    ImGui::DragInt(property.name.c_str(), (int*)property.object);
                    break;
                };

                case Property::Float:
                {
                    ImGui::DragFloat(property.name.c_str(), (float*)property.object);
                    break;
                };

                case Property::Double:
                {
                    ImGui::DragFloat(property.name.c_str(), (float*)property.object);
                    break;
                };

                case Property::Bool:
                {
                    ImGui::Checkbox(property.name.c_str(), (bool*)property.object);
                    break;
                };

                case Property::String:
                {
                    const std::string& text = *((std::string*)property.object);

                    ImGui::Text((property.name + std::string(": ") + (text.empty() ? std::string("None") : text)).c_str());
                    break;
                };

                case Property::Vector2:
                {
                    ImGui::DragFloat2(property.name.c_str(), (float*)property.object, 0.05f);
                    break;
                };
                case Property::Vector3:
                {
                    ImGui::DragFloat3(property.name.c_str(), (float*)property.object, 0.05f);
                    break;
                };

                case Property::Vector4:
                {
                    ImGui::DragFloat4(property.name.c_str(), (float*)property.object, 0.05f);
                    break;
                };

                case Property::Matrix2:
                {
                    ImGui::Text(property.name.c_str());
                    ImGui::DragFloat2("", &((float*)property.object)[0], 0.05f);
                    ImGui::DragFloat2("", &((float*)property.object)[2], 0.05f);
                    break;
                };

                case Property::Matrix3:
                {
                    ImGui::Text(property.name.c_str());
                    ImGui::DragFloat3("", &((float*)property.object)[0], 0.05f);
                    ImGui::DragFloat3("", &((float*)property.object)[3], 0.05f);
                    ImGui::DragFloat3("", &((float*)property.object)[6], 0.05f);
                    break;
                };

                case Property::Matrix4:
                {
                    ImGui::Text(property.name.c_str());
                    ImGui::DragFloat4("", &((float*)property.object)[0], 0.05f);
                    ImGui::DragFloat4("", &((float*)property.object)[4], 0.05f);
                    ImGui::DragFloat4("", &((float*)property.object)[8], 0.05f);
                    ImGui::DragFloat4("", &((float*)property.object)[12], 0.05f);
                    break;
                };

                case Property::Quat:
                {
                    ImGui::DragFloat4(property.name.c_str(), (float*)property.object, 0.05f);
                    break;
                };

                case Property::Color:
                {
                    ImGui::ColorEdit4(property.name.c_str(), (float*)property.object);
                    break;
                };

                case Property::Base:
                {
                    processProperties((Base*)property.object, property.name);
                    break;
                }
                };
            }

            ImGui::TreePop();
        }
    
        base->unbind();
    }
}