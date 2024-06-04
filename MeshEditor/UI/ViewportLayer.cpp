#pragma once

#include "ViewportLayer.h"
#include "../Application/View.h"

#include <spdlog/spdlog.h>

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
        for (auto& callback : m_sizeCallbacks)
        {
            callback(size.x, size.y);
        }

        m_width = size.x;
        m_height = size.y;
    }

    m_wantCaptureMouse = (m_mouse.x >= m_min.x && m_mouse.x <= m_max.x) && (m_mouse.y >= m_min.y && m_mouse.y <= m_max.y);

    ImGui::Image((void*)(intptr_t)m_textureId, { (float)m_width, (float)m_height}, ImVec2(0, 1), ImVec2(1, 0));
   
    ImGui::End();
}

void ViewportLayer::attach(uint32_t textureId)
{
    m_textureId = textureId;
}

bool ViewportLayer::wantCaptureMouse() const
{
    return m_wantCaptureMouse;
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
