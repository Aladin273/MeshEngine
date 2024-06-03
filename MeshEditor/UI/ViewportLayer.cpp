#pragma once

#include "ViewportLayer.h"
#include "../Application/View.h"

ViewportLayer::ViewportLayer(View* view) : BaseLayer(view)
{

}

void ViewportLayer::attach(uint32_t textureId, uint32_t width, uint32_t height)
{
    m_textureId = textureId;
    m_width = width;
    m_height = height;
}

void ViewportLayer::render()
{
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

    auto min = ImGui::GetWindowContentRegionMin();
    auto max = ImGui::GetWindowContentRegionMax();

    auto size = ImGui::GetWindowSize();
    auto position = ImGui::GetWindowPos();

    m_size.x = size.x;
    m_size.y = size.y;
    m_position.x = position.x;
    m_position.y = position.y;

    m_min.x = min.x + position.x;
    m_min.y = min.y + position.y;
    m_max.x = max.x + position.x;
    m_max.y = max.y + position.y;

    ImGui::GetForegroundDrawList()->AddRect({m_min.x, m_min.y }, { m_max.x, m_max.y }, IM_COL32(255, 255, 0, 255));

    // Get the current size of the ImGui window
    ImVec2 imguiWindowSize = ImGui::GetWindowSize();

    // Calculate the aspect ratio of the window and the texture
    float windowAspectRatio = imguiWindowSize.x / imguiWindowSize.y;
    float textureAspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);

    // Calculate the size of the image to be displayed
    ImVec2 imageSize;
    if (windowAspectRatio > textureAspectRatio) 
    {
        // If window is wider than texture, adjust height to fit window
        imageSize.x = imguiWindowSize.y * textureAspectRatio;
        imageSize.y = imguiWindowSize.y;
    }
    else {
        // If window is taller than texture, adjust width to fit window
        imageSize.x = imguiWindowSize.x;
        imageSize.y = imguiWindowSize.x / textureAspectRatio;
    }

    // Display the rendered texture with flipped UV coordinates and adjusted size
    ImGui::Image((void*)(intptr_t)m_textureId, imageSize, ImVec2(0, 1), ImVec2(1, 0));

    ImGui::End();
}

glm::vec2 ViewportLayer::getSize() const
{
    return m_size;
}

glm::vec2 ViewportLayer::getPosition() const
{
    return m_position;
}

glm::vec2 ViewportLayer::getMin() const
{
    return m_min;
}

glm::vec2 ViewportLayer::getMax() const
{
    return m_max;
}
