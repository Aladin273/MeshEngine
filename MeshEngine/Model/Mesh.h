#pragma once

#include <set>
#include <string>

#include "RenderSystem/IRenderSystem.h"
#include "MeshEngine/HalfEdge/HalfEdge.h"

struct bbox
{
    glm::vec3 min{ 0.0f };
    glm::vec3 max{ 0.0f };
};

class Mesh
{
public:
    Mesh(const Utils::HalfEdgeTable& halfEdgeTable);
    Mesh(const Utils::HalfEdgeTable& halfEdgeTable, const Material& material);

    void render(IRenderSystem& rs);

    void update();
    void update(glm::vec3 orig, glm::vec3 target);

    void updateBbox();
    void updateLines();
    void updateHoles();
    void updateTriangles();
    void updateBoundaryFaces();

    // Center based
    void applyTransformation(Utils::VertexHandle fh, const glm::mat4& trf);
    // Center based
    void applyTransformation(Utils::FaceHandle fh, const glm::mat4& trf);

    void deleteFace(Utils::FaceHandle fh);

    const bbox& getBoundingBox();
    const Utils::HalfEdgeTable& getHalfEdgeTable() const;

    void setName(const std::string& name);
    void setMaterial(const Material& material);

    const std::string& getName() const;
    const Material& getMaterial() const;

    void setDrawBoundaries(bool enable);
    void setDrawTriangles(bool enable);
    void setDrawLines(bool enable);
    void setDrawHoles(bool enable);

    bool getDrawBoundaries() const;
    bool getDrawTriangles() const;
    bool getDrawLines() const;
    bool getDrawHoles() const;

    void setColorBoundaries(glm::vec3 color);
    void setColorTriangles(glm::vec3 color);
    void setColorLines(glm::vec3 color);
    void setColorHoles(glm::vec3 color);

    const glm::vec3& getColorBoundaries();
    const glm::vec3& getColorTriangles();
    const glm::vec3& getColorLines();
    const glm::vec3& getColorHoles();

private:
    std::string m_name = "Mesh 0";

    bool m_drawBoundaries = false;
    bool m_drawTriangles = true;
    bool m_drawLines = false;
    bool m_drawHoles = false;

    Utils::HalfEdgeTable m_table;
    std::vector<Vertex> m_lines;
    std::vector<Vertex> m_holes;
    std::vector<Vertex> m_triangles;
    std::vector<Vertex> m_boundaries;

    bbox m_bbox;
    Material m_material;
    glm::vec3 m_colorLines{ 1.0f };
    glm::vec3 m_colorHoles{ 0.75f, 0.25f, 0.25f };
    glm::vec3 m_colorTriangles{ 0.25f, 0.75f, 0.25f };
    glm::vec3 m_colorBoundaries{ 0.75f, 0.25f, 0.25f };
};


