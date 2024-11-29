#pragma once

#include "ViewportLayer.h"
#include "MeshEditor/Editor/View.h"

ViewportLayer::ViewportLayer(View* view) : BaseLayer(view)
{

}

void ViewportLayer::render()
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
        m_sizeCallback(size.x, size.y);

        m_width = size.x;
        m_height = size.y;
    }

    bool isActiveWindow = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);

    m_wantCaptureMouse = isActiveWindow && ((m_mouse.x >= m_min.x && m_mouse.x <= m_max.x) && (m_mouse.y >= m_min.y && m_mouse.y <= m_max.y));
    m_wantCaptureKeyboard = isActiveWindow;

    ImGui::Image((void*)(intptr_t)frameTextureId, { (float)m_width, (float)m_height }, ImVec2(0, 1), ImVec2(1, 0));

    guizmo();
    overlay();

    ImGui::End();
}

bool ViewportLayer::wantCaptureMouse() const
{
    return m_wantCaptureMouse;
}

bool ViewportLayer::wantCaptureKeyboard() const
{
    return m_wantCaptureKeyboard;
}

bool ViewportLayer::wantCaptureGizmo() const
{
    return m_wantCaptureGizmo;
}

void ViewportLayer::remapToRelative(double& x, double& y)
{
    x = m_mouse.x - m_min.x;
    y = m_height - (m_mouse.y - m_min.y);
}

bool ViewportLayer::getGizmoVisible() const
{
    return m_gizmoVisible;
}

void ViewportLayer::setGizmoVisible(bool visible)
{
    m_gizmoVisible = visible;
}

GizmoSpace ViewportLayer::getGizmoSpace() const
{
    return m_gizmoSpace;
}

GizmoMode ViewportLayer::getGizmoMode() const
{
    return m_gizmoMode;
}

void ViewportLayer::setGizmoSpace(GizmoSpace space)
{
    m_gizmoSpace = space;
}

void ViewportLayer::setGizmoMode(GizmoMode mode)
{
    m_gizmoMode = mode;
}

void ViewportLayer::setGizmoTransform(const glm::mat4& transform)
{
    m_gizmoTransform = transform;
}

void ViewportLayer::setGizmoCallback(const GizmoCallback& callback)
{
    m_gizmoCallback = callback;
}

void ViewportLayer::switchGizmoMode()
{
    m_gizmoMode = (GizmoMode)(((uint8_t)m_gizmoMode + 1) % (uint8_t)GizmoMode::MAX);
}

void ViewportLayer::switchGizmoSpace()
{
    m_gizmoSpace = (GizmoSpace)(((uint8_t)m_gizmoSpace + 1) % (uint8_t)GizmoSpace::MAX);
}

void ViewportLayer::setFramebufferSizeCallback(const FramebufferSizeCallback& callback)
{
    m_sizeCallback = callback;
}

void ViewportLayer::guizmo()
{
    if (m_gizmoVisible && m_gizmoMode != GizmoMode::Select)
    {
        ImGuizmo::Enable(true);

        ImGuizmo::SetOrthographic(m_view->getViewport().getOrthogonal());
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(m_min.x, m_min.y, m_width, m_height);

        ImGuizmo::OPERATION operation;
        ImGuizmo::MODE mode;

        if (m_gizmoMode == GizmoMode::Translate)
            operation = ImGuizmo::TRANSLATE;
        else if (m_gizmoMode == GizmoMode::Rotate)
            operation = ImGuizmo::ROTATE;
        else if (m_gizmoMode == GizmoMode::Scale)
            operation = ImGuizmo::SCALE;

        if (m_gizmoSpace == GizmoSpace::World)
            mode = ImGuizmo::WORLD;
        else if (m_gizmoSpace == GizmoSpace::Local)
            mode = ImGuizmo::LOCAL;

        m_gizmoDelta = glm::mat4(1.0f);
        m_gizmoTranslation = glm::vec3(1.0f);
        m_gizmoRotation = glm::vec3(1.0f);
        m_gizmoScale = glm::vec3(1.0f);

        if (ImGuizmo::Manipulate(glm::value_ptr(m_view->getViewport().getCamera().calcViewMatrix()), glm::value_ptr(m_view->getViewport().calcProjectionMatrix()),
            operation, mode, glm::value_ptr(m_gizmoTransform), glm::value_ptr(m_gizmoDelta)))
        {   
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(m_gizmoDelta), glm::value_ptr(m_gizmoTranslation), glm::value_ptr(m_gizmoRotation), glm::value_ptr(m_gizmoScale));
            m_gizmoCallback(m_gizmoTransform, m_gizmoDelta, m_gizmoTranslation, m_gizmoRotation, m_gizmoScale);
        }

        m_wantCaptureGizmo = ImGuizmo::IsOver() || ImGuizmo::IsUsing() || ImGuizmo::IsUsingAny();
    }
    else
    {
        ImGuizmo::Enable(false);
        m_wantCaptureGizmo = false;
    }
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
