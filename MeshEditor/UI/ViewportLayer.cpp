#pragma once

#include "ViewportLayer.h"
#include "../Application/View.h"

#include <ImGuizmo.h>

ViewportLayer::ViewportLayer(View* view) : BaseLayer(view)
{

}

void ViewportLayer::render()
{
    viewport();
    overlay();
}

void ViewportLayer::attach(uint32_t textureId)
{
    m_textureId = textureId;
}

bool ViewportLayer::wantCaptureMouse() const
{
    return m_wantCaptureMouse;
}

bool ViewportLayer::wantCaptureKeyboard() const
{
    return m_wantCaptureKeyboard;
}

void ViewportLayer::remapToRelative(double& x, double& y)
{
    x = m_mouse.x - m_min.x;
    y = m_height - (m_mouse.y - m_min.y);
}

void ViewportLayer::setFramebufferSizeCallback(const FramebufferSizeCallback& callback)
{
    m_sizeCallbacks.push_back(callback);
}

void ViewportLayer::viewport()
{
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

    auto mouse = ImGui::GetMousePos();
    auto position = ImGui::GetWindowPos();
    auto min = ImGui::GetWindowContentRegionMin();
    auto max = ImGui::GetWindowContentRegionMax();

    m_mouse = { mouse.x, mouse.y };
    m_position = { position.x, position.y };

    m_min = glm::vec2(min.x, min.y) + m_position;
    m_max = glm::vec2(max.x, max.y) + m_position;

    auto size = m_max - m_min;

    if (size.x != m_width || size.y != m_height)
    {
        for (auto& callback : m_sizeCallbacks)
        {
            callback(size.x, size.y);
        }

        m_width = size.x;
        m_height = size.y;
    }

    bool isActiveWindow = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
    
    m_wantCaptureMouse = isActiveWindow && ((m_mouse.x >= m_min.x && m_mouse.x <= m_max.x) && (m_mouse.y >= m_min.y && m_mouse.y <= m_max.y));
    m_wantCaptureKeyboard = isActiveWindow;

    ImGui::Image((void*)(intptr_t)m_textureId, { (float)m_width, (float)m_height }, ImVec2(0, 1), ImVec2(1, 0));

    if (m_view->getSelected())
    {
        ImGuizmo::SetOrthographic(m_view->getViewport().getOrthogonal());
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_position.x, m_position.y, m_width, m_height);

        glm::mat4 transform = m_view->getSelected()->getRelativeTransform();

        ImGuizmo::Manipulate(&(m_view->getViewport().getCamera().calcViewMatrix()[0][0]), &(m_view->getViewport().calcProjectionMatrix()[0][0]), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, &(transform)[0][0]);

        m_view->getSelected()->setRelativeTransform(transform);
        m_view->selectBlocked = ImGuizmo::IsUsing() || ImGuizmo::IsOver();
    }

    ImGui::End();
}

void ViewportLayer::overlay()
{
    static int location = 0;
    ImGuiIO& io = ImGui::GetIO();
    
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration 
        | ImGuiWindowFlags_NoDocking 
        | ImGuiWindowFlags_AlwaysAutoResize 
        | ImGuiWindowFlags_NoSavedSettings 
        | ImGuiWindowFlags_NoFocusOnAppearing 
        | ImGuiWindowFlags_NoNav;

    if (location >= 0)
    {
        const float PAD = 20.0f;
        
        ImVec2 work_pos = { m_min.x, m_min.y };
        ImVec2 work_size = { (float)m_width, (float)m_height };
        ImVec2 window_pos, window_pos_pivot;
        
        window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
        window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
        window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
        window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    else if (location == -2)
    {
        ImVec2 window_pos;
        
        window_pos.x = (m_max.x + m_min.x) / 2;
        window_pos.y = (m_max.y + m_min.y) / 2;

        ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        window_flags |= ImGuiWindowFlags_NoMove;
    }
    
    ImGui::SetNextWindowBgAlpha(0.2f);

    if (ImGui::Begin("Overlay", nullptr, window_flags))
    {
        ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
        ImGui::Text("%.1f ms", 1000.0f / ImGui::GetIO().Framerate);

        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
            if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
            if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
            if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
            if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
            if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}
