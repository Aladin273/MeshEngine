#pragma once

#include <set>
#include <string>

#include "RenderSystem/IRenderSystem.h"
#include "HalfEdge/HalfEdge.h"

struct bbox
{
    glm::vec3 min{ 0.0f };
    glm::vec3 max{ 0.0f };
};

class Mesh
{
public:
    bool renderTriangles = true;
    bool renderLines = false;
    bool renderHoles = false;
    bool renderBoundaries = false;
    bool renderBbox = false;

    glm::vec4 colorTriangles{ 0.25f, 0.75f, 0.25f, 1.0f };
    glm::vec4 colorLines{ 1.0f };
    glm::vec4 colorHoles{ 0.75f, 0.25f, 0.25f, 1.0f };
    glm::vec4 colorBoundaries{ 0.75f, 0.25f, 0.25f, 1.0f };
    glm::vec4 colorBboxData{ 1.0f };

public:
    Mesh(const heds::HalfEdgeTable& halfEdgeTable);
    Mesh(const heds::HalfEdgeTable& halfEdgeTable, const Material& material);

    void render(IRenderSystem& rs);

    void applyTransformation(heds::FaceHandle fh, const glm::mat4& trf);
    void applyTransformation(heds::VertexHandle fh, const glm::mat4& trf);
    
    void deleteFace(heds::FaceHandle fh);
    void deleteVertex(heds::VertexHandle fh);

    const bbox& getBoundingBox();
    const heds::HalfEdgeTable& getHalfEdgeTable() const;

    void setName(const std::string& name);
    void setMaterial(const Material& material);

    const std::string& getName() const;
    const Material& getMaterial() const;

private:
    void update();

    std::string m_name = "Mesh";

    bbox m_bbox;
    Material m_material;

    heds::HalfEdgeTable m_table;
    std::vector<Vertex> m_lines;
    std::vector<Vertex> m_holes;
    std::vector<Vertex> m_triangles;
    std::vector<Vertex> m_boundaries;
    std::vector<Vertex> m_bboxData;
};