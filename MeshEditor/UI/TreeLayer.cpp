#include "TreeLayer.h"
#include "../Application/View.h"

TreeLayer::TreeLayer(View* view) : BaseLayer(view)
{
}

void TreeLayer::render()
{
    ImGui::Begin("Tree");

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