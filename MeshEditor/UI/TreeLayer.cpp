#include "TreeLayer.h"

#include "MeshEditor/Editor/View.h"
#include "MeshEditor/Editor/Editor.h"

#include "MeshEngine/Node/MeshNode.h"
#include "MeshEngine/Node/SpriteNode.h"

#include "MeshEngine/Node/DirLightNode.h"
#include "MeshEngine/Node/PointLightNode.h"
#include "MeshEngine/Node/SpotLightNode.h"

#include "MeshEngine/Misc/Settings.h"

#include <Windows.h>
#include <commdlg.h>

TreeLayer::TreeLayer(View* view) : BaseLayer(view)
{
}

void TreeLayer::render()
{
    ImGui::Begin("Tree");

    std::string filePath;

    if (ImGui::Button("Mesh", { ImGui::GetContentRegionAvail().x / 3, 20 }))
    {
        if (ImGui::BeginChild("Add from file"))
        {
            std::string filePath = renderDialog();

            if (!filePath.empty())
            {
                m_view->getScene()->attachNode(Editor::instance()->loadModel(filePath));
                m_view->getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
                m_view->zoomToFit();
            }

            ImGui::EndChild();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Sprite", { ImGui::GetContentRegionAvail().x / 2, 20 }))
    {
        if (ImGui::BeginChild("Add from file"))
        {
            std::string filePath = renderDialog();

            if (!filePath.empty())
            {
                Material material;
                material.diffuseMap.path = filePath;

                std::unique_ptr<SpriteNode> spriteNode = std::make_unique<SpriteNode>();
                spriteNode->attachSprite(std::make_unique<Sprite>(material));
                spriteNode->setName(filePath);

                m_view->getScene()->attachNode(std::move(spriteNode));
                m_view->getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
                m_view->zoomToFit();
            }

            ImGui::EndChild();
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Remove", { ImGui::GetContentRegionAvail().x / 1, 20 }))
    {
        m_view->getScene()->detachNode(m_view->getSelected());

        m_view->setSelected(nullptr);
        m_selectedNode = nullptr;
    }

    if (ImGui::Button("DirLight", { ImGui::GetContentRegionAvail().x / 3, 20 }))
    {
        std::unique_ptr<DirLightNode> lightNode = std::make_unique<DirLightNode>();
        lightNode->setRelativeTransform(glm::translate(glm::vec3(0.f, 10.f, 0.f)) * glm::rotate(glm::radians(60.f), glm::vec3(1.f, -0.25f, -1.f)));

        m_view->getScene()->attachNode(std::move(lightNode));
        m_view->getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
        m_view->zoomToFit();
    }

    ImGui::SameLine();

    if (ImGui::Button("PointLight", { ImGui::GetContentRegionAvail().x / 2, 20 }))
    {
        std::unique_ptr<PointLightNode> lightNode = std::make_unique<PointLightNode>();
        lightNode->setRelativeTransform(glm::translate(glm::vec3(0.f, 10.f, 0.f)));

        m_view->getScene()->attachNode(std::move(lightNode));
        m_view->getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
        m_view->zoomToFit();
    }

    ImGui::SameLine();

    if (ImGui::Button("SpotLight", { ImGui::GetContentRegionAvail().x / 1, 20 }))
    {
        std::unique_ptr<SpotLightNode> lightNode = std::make_unique<SpotLightNode>();
        lightNode->setRelativeTransform(glm::translate(glm::vec3(0.f, 10.f, 0.f)));

        m_view->getScene()->attachNode(std::move(lightNode));
        m_view->getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
        m_view->zoomToFit();
    }

    ImGui::Separator();
    ImGui::Spacing();

    for (auto& node : m_view->getScene()->getNodes())
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

std::string TreeLayer::renderDialog()
{
    std::string filePath = "";

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

    return filePath;
}
