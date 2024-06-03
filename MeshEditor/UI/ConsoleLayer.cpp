#pragma once

#include "ConsoleLayer.h"
#include "../Application/View.h"

#include <spdlog/spdlog.h>
#include <MeshEngine/Logger/mesh_engine_sink.h>

std::shared_ptr<spdlog::sinks::mesh_engine_sink_mt> g_sink;

ConsoleLayer::ConsoleLayer(View* view) : BaseLayer(view)
{
    auto it = std::find_if(spdlog::default_logger()->sinks().begin(), spdlog::default_logger()->sinks().end(), [](const auto& sink)
    {
        return dynamic_cast<spdlog::sinks::mesh_engine_sink_mt*>(sink.get()) != nullptr;
    });

    g_sink = std::dynamic_pointer_cast<spdlog::sinks::mesh_engine_sink_mt>(*it);
}

void ConsoleLayer::render()
{
    if (ImGui::Begin("Console"))
    {
        char inputBuffer[256];
        inputBuffer[0] = '\0';

        if (ImGui::InputText("##ConsoleInput", inputBuffer, IM_ARRAYSIZE(inputBuffer), ImGuiInputTextFlags_EnterReturnsTrue))
        {
            spdlog::info(inputBuffer);
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear"))
        {
            g_sink->clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Copy"))
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

        if (g_sink->scrollToBottom)
        {
            ImGui::SetScrollHereY(1.0f);
            g_sink->scrollToBottom = false;
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
