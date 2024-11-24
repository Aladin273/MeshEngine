#pragma once

#include "Node.h"

#include "MeshEngine/Base/Mesh.h"

class MeshNode : public Node
{
public:
    MeshNode();
    virtual ~MeshNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Base, "m_mesh", *m_mesh.get());
        
        super::bind();
    }

public:
    virtual const BoundingBox& getBoundingBox() const override;

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

public:
    void attachMesh(std::unique_ptr<Mesh> mesh);
    Mesh* getMesh() const;

protected:
    uint32_t m_renderTrianglesId = 0;
    uint32_t m_renderLinesId = 0;
    uint32_t m_renderHolesId = 0;
    uint32_t m_renderBoundariesId = 0;

protected:
    std::unique_ptr<Mesh> m_mesh;
};
