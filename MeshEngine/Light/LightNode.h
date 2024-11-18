#pragma once

#include "MeshEngine/Model/Node.h"
#include "MeshEngine/Base/Mesh.h"

class LightNode : public Node
{
public:
    LightNode();
    virtual ~LightNode();

public:
    virtual const BoundingBox& getBoundingBox() const override;

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

protected:
    uint32_t m_renderId = 0;
    glm::vec3 m_renderColor;
    std::unique_ptr<Mesh> m_renderMesh;
};
