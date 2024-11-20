#pragma once

#include "MeshEngine/Node/Node.h"

class Mesh;
class View;

class PlaneNode : public Node
{
public:
	PlaneNode(View* view, glm::vec3 worldUp, float width, float height, uint32_t numSubs);
    virtual ~PlaneNode();

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    View* m_view;

    uint32_t m_renderId = 0;
    std::unique_ptr<Mesh> m_mesh;
};