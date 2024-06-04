#pragma once

#include "ConsoleLayer.h"

#include <imgui_internal.h>

#include "../Application/View.h"
#include "MeshEngine/Logger/Logger.h"

std::shared_ptr<MeshEngine::Logger::ringbuffer_sink_mt> g_sink;

ConsoleLayer::ConsoleLayer(View* view) : BaseLayer(view)
{
    g_sink = MeshEngine::Logger::ringbuffer_sink();
}

void ConsoleLayer::render()
{
    if (ImGui::Begin("Console"))
    {
        char inputBuffer[256];
        inputBuffer[0] = '\0';

        if (ImGui::InputTextEx("##ConsoleInput", nullptr, inputBuffer, IM_ARRAYSIZE(inputBuffer), ImVec2(ImGui::GetContentRegionAvail().x * 0.8f, 20.f), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            MeshEngine::Logger::info(inputBuffer);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear", ImVec2(ImGui::GetContentRegionAvail().x * 0.5, 20.f)))
        {
            g_sink->clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Copy", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)))
        {
            std::string logs;
            for (const auto& log : g_sink->formatted())
            {
                logs += log + "\n";
            }
            ImGui::SetClipboardText(logs.c_str());
        }

        ImGui::BeginChild("ScrollingRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        for (const auto& item : g_sink->formatted())
        {
            ImGui::TextUnformatted(item.c_str());
        }

        if (g_sink->updated())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
