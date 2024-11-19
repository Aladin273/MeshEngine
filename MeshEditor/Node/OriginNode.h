#pragma once

#include "MeshEngine/Node/MeshNode.h"

class View;

class OriginNode : public MeshNode
{
public:
    OriginNode(View* view);
    virtual ~OriginNode();

public:
    virtual void update(float deltaTime) override;

private:
    View* m_view;
};