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
            ofn.lpstrFilter = "STL Files (*.stl)\0*.stl\0DAE Files (*.dae)\0*.dae\0All Files (*.*)\0*.*\0";
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
        m_view->getSelected()->deleteFromParent();
        m_view->getModel()->detachNode(m_view->getSelected());

        m_selectedNode = nullptr;
        m_view->setSelected(nullptr);
    }

    ImGui::Separator();
    ImGui::Spacing();

    for (auto& node : m_view->getModel()->getNodes())
    {
        renderNode(node.get(), m_selectedNode);
    }

    ImGui::End();
}

void TreeLayer::renderNode(Node* node, Node*& selected)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | (node == selected ? ImGuiTreeNodeFlags_Selected : 0);

    bool nodeOpen = ImGui::TreeNodeEx(node, nodeFlags, "%s", node->getName().c_str());

    if (ImGui::IsItemClicked())
    {
        selected = node;
        m_view->setSelected(selected);
    }

    if (nodeOpen)
    {
        for (const auto& child : node->getChildren())
        {
            renderNode(child.get(), selected);
        }

        ImGui::TreePop();
    }
}