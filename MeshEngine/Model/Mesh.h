#pragma once

#include <string>
#include <memory>

#include "MeshEngine/RenderSystem/RenderSystem.h"
#include "MeshEngine/RenderSystem/Shader.h"
#include "MeshEngine/HalfEdge/HalfEdge.h"

struct BoundaryBox
{
    glm::vec3 min{ 0.0f };
    glm::vec3 max{ 0.0f };
};

struct Texture
{
    uint32_t id;
    std::string type;
    std::string path;
};

struct Material
{
    glm::vec3 ambient{ 0.0f };
    glm::vec4 diffuse{ 1.0f };
    glm::vec3 specular{ 1.0f };
    glm::vec3 emission{ 0.0f };
    float shininess{ 128.0f };

    Texture diffuseMap  {};
    Texture specularMap {};
    Texture emissionMap {};
};

class Mesh
{
public:
    bool renderTriangles = true;
    bool renderLines = false;
    bool renderHoles = false;
    bool renderBoundaries = false;

    glm::vec4 colorTriangles{ 0.25f, 0.75f, 0.25f, 1.0f };
    glm::vec4 colorLines{ 0.0f, 0.0f, 0.0f, 1.0f };
    glm::vec4 colorHoles{ 0.75f, 0.25f, 0.25f, 1.0f };
    glm::vec4 colorBoundaries{ 0.75f, 0.25f, 0.25f, 1.0f };

public:
    Mesh(const heds::HalfEdgeTable<Vertex>& halfEdgeTable);
    Mesh(const heds::HalfEdgeTable<Vertex>& halfEdgeTable, const Material& material);

    void render(RenderSystem& rs, Shader& shader);

    void applyTransformation(heds::FaceHandle fh, const glm::mat4& trf);
    void applyTransformation(heds::VertexHandle fh, const glm::mat4& trf);
    
    void deleteFace(heds::FaceHandle fh);

    const BoundaryBox& getBoundingBox();
    const heds::HalfEdgeTable<Vertex>& getHalfEdgeTable() const;

    const std::string& getName() const;
    std::string& getName();
    
    const Material& getMaterial() const;
    Material& getMaterial();

    void setName(const std::string& name);
    void setMaterial(const Material& material);

    static std::unique_ptr<Mesh> createCube(glm::vec3 center, float length);
    static std::unique_ptr<Mesh> createCone(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createCylinder(glm::vec3 dir, float R, float h, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createTorus(glm::vec3 dir, float minorRadius, float majorRadius, uint32_t majorSegments);
    static std::unique_ptr<Mesh> createArrow(glm::vec3 dir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions);
    static std::unique_ptr<Mesh> createPlane(glm::vec3 dir, float width, float heigth, uint32_t numSubdivisions);

private:
    void update();

    std::string m_name = "Mesh";

    BoundaryBox m_bbox;
    Material m_material;

    heds::HalfEdgeTable<Vertex> m_table;
    
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_triangles;
    std::vector<uint32_t> m_lines;
    std::vector<uint32_t> m_holes;
    std::vector<uint32_t> m_boundaries;

    bool m_bufferData = true;
    uint32_t m_trianglesId = 0;
    uint32_t m_linesId = 0;
    uint32_t m_holesId = 0;
    uint32_t m_boundariesId = 0;

    bool m_bufferSubData = false;
    std::vector<uint32_t> m_subDataIndices;
};