#pragma once

#include "MeshEngine/Node/Node.h"

class View;

class OriginNode : public Node
{
public:
    OriginNode(View* view);
    virtual ~OriginNode();

public:
    virtual void update(float deltaTime) override;

private:
    View* m_view;
};