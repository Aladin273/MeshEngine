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
        bindPropertyEx(Property::Base, "Mesh", *m_mesh.get());
        
        super::bind();
    }

public:
    virtual const BoundingBox& getBoundingBox() const override;

public:
    void attachMesh(std::unique_ptr<Mesh> mesh);
    Mesh* getMesh() const;

protected:
    std::unique_ptr<Mesh> m_mesh;
};
