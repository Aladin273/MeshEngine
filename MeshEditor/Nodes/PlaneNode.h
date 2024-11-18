#pragma once

#include "MeshEngine/Model/MeshNode.h"

class View;

class PlaneNode : public MeshNode
{
public:
	PlaneNode(View* view, glm::vec3 worldUp, float width, float height, uint32_t numSubs);
    virtual ~PlaneNode();

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;

private:
    View* m_view;
};