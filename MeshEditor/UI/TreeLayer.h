#pragma once

#include "BaseLayer.h"

class Node;

class TreeLayer : public BaseLayer
{
public:
    TreeLayer(View* view);

    void render() override;

private:
    void renderNode(Node* node, Node*& selected);

    Node* m_selectedNode = nullptr;
};