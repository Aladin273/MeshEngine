#pragma once

#include "MeshEngine/Model/MeshNode.h"

class View;

class ArrowNode : public MeshNode
{
public:
    ArrowNode(View* view);
    virtual ~ArrowNode();

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;

private:
    View* m_view;
};