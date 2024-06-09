#include "TreeLayer.h"
#include "../Application/View.h"

#include "../Application/Settings.h"
#include "../Application/Application.h"

#include <Windows.h>
#include <commdlg.h>

TreeLayer::TreeLayer(View* view) : BaseLayer(view)
{
}

void TreeLayer::render()
{
    ImGui::Begin("Tree");

    std::string filePath;

    if (ImGui::Button("Add", { ImGui::GetContentRegionAvail().x / 2, 20 }))
    {
        if (ImGui::BeginChild("Add from file"))
        {
            char exePath[MAX_PATH];
            GetModuleFileName(NULL, exePath, MAX_PATH);
            std::string initialDir = exePath;
            size_t lastSlash = initialDir.find_last_of("\\");
            initialDir = initialDir.substr(0, lastSlash);

            char filename[MAX_PATH] = "";
            OPENFILENAME ofn;
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = NULL;
            ofn.lpstrFile = filename;
            ofn.lpstrFile[0] = '\0';
            ofn.nMaxFile = sizeof(filename);
            ofn.lpstrInitialDir = initialDir.c_str();
            //ofn.lpstrFilter = "STL Files (*.stl)\0*.stl\0DAE Files (*.dae)\0*.dae\0All Files (*.*)\0*.*\0";
            ofn.lpstrFilter = "";
            ofn.nFilterIndex = 1;
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

            if (GetOpenFileName(&ofn))
            {
                filePath = ofn.lpstrFile;
            }

            if (!filePath.empty())
            {
                auto model = Application::instance()->loadModel(filePath);

                for (auto& node : model->getNodes())
                    m_view->getModel()->attachNode(std::move(node));

                m_view->getViewport().getCamera().setEyeTargetUp(Settings::eye, Settings::target, Settings::up);
                m_view->zoomToFit();
            }

            ImGui::EndChild();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Remove", { ImGui::GetContentRegionAvail().x, 20 }))
    {   
        m_view->requestDelete(m_view->getSelected());
        
        m_view->setSelected(nullptr);
        m_selectedNode = nullptr;
    }

    ImGui::Separator();
    ImGui::Spacing();

    for (auto& node : m_view->getModel()->getNodes())
    {
        renderNode(node.get());
    }

    ImGui::End();
}

void TreeLayer::renderNode(Node* node)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (node == m_view->getSelected() ? ImGuiTreeNodeFlags_Selected : 0);

    auto open = ImGui::TreeNodeEx(node, nodeFlags, "%s", node->getName().c_str());

    if (ImGui::IsItemClicked())
    {
        m_view->setSelected(node);
    }

    if (open)
    {
        for (const auto& child : node->getChildren())
        {
            renderNode(child.get());
        }

        ImGui::TreePop();
    }
}