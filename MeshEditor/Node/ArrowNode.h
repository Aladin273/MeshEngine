#pragma once

#include "MeshEngine/Node/Node.h"

class Mesh;
class View;

class ArrowNode : public Node
{
public:
    ArrowNode(View* view, glm::vec3 axis, glm::vec3 color);
    virtual ~ArrowNode();

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    View* m_view;

    uint32_t m_renderId = 0;
    glm::vec3 m_renderColor{ 1.f };
    std::unique_ptr<Mesh> m_mesh;
};