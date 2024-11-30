#pragma once

#include <string>
#include <memory>

#include "MeshEngine/Math/HalfEdge.h"
#include "MeshEngine/Math/BoundingBox.h"

#include "MeshEngine/Base/RenderBase.h"
#include "MeshEngine/Base/Texture.h"
#include "MeshEngine/Base/Material.h"

class Mesh : public RenderBase
{
public:
    Mesh(const HalfEdgeTable<Vertex>& halfEdgeTable);
    Mesh(const HalfEdgeTable<Vertex>& halfEdgeTable, const Material& material);

public:
    virtual void bind() override
    {
        bindProperty(m_material);

        super::bind();
    };

public:
    virtual void updateData() override;
    virtual void updateSubData() override;

    virtual void updateBbox();

public:
    void applyTransformation(HalfEdgeFaceHandle fh, const glm::mat4& trf);
    void applyTransformation(HalfEdgeVertexHandle fh, const glm::mat4& trf);
    
    void deleteFace(HalfEdgeFaceHandle fh);

    const BoundingBox& getBoundingBox() const;
    HalfEdgeTable<Vertex>& getHalfEdgeTable();
    
    const Material& getMaterial() const;
    void setMaterial(const Material& material);

    static std::unique_ptr<Mesh> createCube(glm::vec3 center, float length);
    static std::unique_ptr<Mesh> createCone(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createCylinder(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createTorus(glm::vec3 dir, float minorRadius, float majorRadius, uint32_t majorSegments);
    static std::unique_ptr<Mesh> createArrow(glm::vec3 dir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createPlane(glm::vec3 dir, float width, float heigth, uint32_t numSubdivisions);

private:
    BoundingBox m_bbox;
    Material m_material;
    HalfEdgeTable<Vertex> m_table;
};