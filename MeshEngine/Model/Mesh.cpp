#include "Mesh.h"

#include <numeric>

Mesh::Mesh(const heds::HalfEdgeTable& halfEdgeTable)
    : m_table(halfEdgeTable)
{
    update();
}

Mesh::Mesh(const heds::HalfEdgeTable& halfEdgeTable, const Material& material)
    : m_table(halfEdgeTable), m_material(material)
{
    update();
}

void Mesh::render(RenderSystem& rs, Shader& shader)
{
    if (m_vertices.empty())
        return;

    if (m_bufferData)
    {
        m_bufferData = false;
        
        rs.unbufferData(m_bufferTriangles);
        rs.unbufferData(m_bufferLines);
        rs.unbufferData(m_bufferHoles);
        rs.unbufferData(m_bufferBoundaries);
       
        m_bufferTriangles = rs.bufferData(m_vertices, m_triangles);
        m_bufferLines = rs.bufferData(m_vertices, m_lines);
        m_bufferHoles = rs.bufferData(m_vertices, m_holes);
        m_bufferBoundaries = rs.bufferData(m_vertices, m_boundaries);
    }

    if (m_bufferSubData)
    {
        m_bufferSubData = false;

        for (auto& indice : m_subDataIndices)
        {
            rs.bufferSubData(m_bufferTriangles, indice, m_vertices[indice]);
            rs.bufferSubData(m_bufferLines, indice, m_vertices[indice]);
            rs.bufferSubData(m_bufferHoles, indice, m_vertices[indice]);
            rs.bufferSubData(m_bufferBoundaries, indice, m_vertices[indice]);
        }
    }
    
    shader.bind();
    
    if (renderTriangles && !m_triangles.empty())
    {
        shader.setVec3("material.ambient", m_material.ambient);
        shader.setVec3("material.diffuse", glm::vec3(m_material.diffuse));
        shader.setVec3("material.specular", m_material.specular);
        shader.setVec3("material.emission", m_material.emission);
        shader.setFloat("material.shininess", m_material.shininess);

        rs.bindBuffer(m_bufferTriangles);
        rs.renderTriangles();
    }

    if (renderLines && !m_lines.empty())
    {
        shader.setVec3("material.ambient", glm::vec3(0.f));
        shader.setVec3("material.diffuse", glm::vec3(0.f));
        shader.setVec3("material.specular", glm::vec3(0.f));
        shader.setVec3("material.emission", colorLines);
        shader.setFloat("material.shininess", 0);

        rs.bindBuffer(m_bufferLines);
        
        rs.setLineSize(2.0f);
        rs.renderLines();
    }

    if (renderHoles && !m_holes.empty())
    {
        shader.setVec3("material.ambient", glm::vec3(0.f));
        shader.setVec3("material.diffuse", glm::vec3(0.f));
        shader.setVec3("material.specular", glm::vec3(0.f));
        shader.setVec3("material.emission", colorHoles);
        shader.setFloat("material.shininess", 0);

        rs.bindBuffer(m_bufferHoles);

        rs.setLineSize(4.0f);
        rs.renderLines();
    }

    if (renderBoundaries && !m_boundaries.empty())
    {
        shader.setVec3("material.ambient", glm::vec3(0.f));
        shader.setVec3("material.diffuse", glm::vec3(0.f));
        shader.setVec3("material.specular", glm::vec3(0.f));
        shader.setVec3("material.emission", colorBoundaries);
        shader.setFloat("material.shininess", 0);

        rs.bindBuffer(m_bufferBoundaries);
        rs.renderTriangles();
    }

    rs.unbindBuffer();

    //shader.setInt("material.diffuseMap", 0);
    //shader.setInt("material.specularMap", 1);
    //shader.setInt("material.emissionMap", 2);

    //rs.bindTexture(0, m_material.diffuseMap);
    //rs.bindTexture(1, m_material.specularMap);
    //rs.bindTexture(2, m_material.emissionMap);
    
    //rs.unbindTexture();
}

void Mesh::update()
{
    m_bufferData = true;
    m_vertices.clear();
    m_triangles.clear();
    m_lines.clear();
    m_holes.clear();
    m_boundaries.clear();

    if (m_table.getVertices().empty()) return;

    const auto& vertices = m_table.getVertices();
    const auto& faces = m_table.getFaces();

    m_vertices.reserve(vertices.size());
    m_triangles.reserve(faces.size() * 6);
    m_lines.reserve(faces.size() * 8);
    m_holes.reserve(faces.size() * 8);
    m_boundaries.reserve(faces.size() * 6);

    m_bbox.min = m_bbox.max = vertices.front().data;

    for (const auto& vertex : vertices)
    {
        const auto& position = vertex.data;

        m_bbox.min.x = std::min(m_bbox.min.x, position.x);
        m_bbox.min.y = std::min(m_bbox.min.y, position.y);
        m_bbox.min.z = std::min(m_bbox.min.z, position.z);

        m_bbox.max.x = std::max(m_bbox.max.x, position.x);
        m_bbox.max.y = std::max(m_bbox.max.y, position.y);
        m_bbox.max.z = std::max(m_bbox.max.z, position.z);

        m_vertices.push_back({ position, {}, {} });
    }

    std::vector<std::pair<glm::vec3, float>> normalsMap{ 0 };
    normalsMap.resize(vertices.size());

    for (const auto& face : faces)
    {
        heds::VertexHandle vhs[4];
        heds::HalfEdgeHandle hehs[4];

        hehs[0] = face.heh;
        for (int i = 1; i < 4; ++i) 
            hehs[i] = m_table.next(hehs[i - 1]);

        for (int i = 0; i < 4; ++i)
            vhs[i] = m_table.sourceVertex(hehs[i]);

        glm::vec3 ab = m_vertices[vhs[1]].position - m_vertices[vhs[0]].position;
        glm::vec3 bc = m_vertices[vhs[2]].position - m_vertices[vhs[1]].position;
        glm::vec3 normal = glm::cross(ab, bc);

        for (const auto& vh : vhs)
        {
            normalsMap[vh].first += normal;
            normalsMap[vh].second += 1;
        }

        m_triangles.push_back(vhs[0]);
        m_triangles.push_back(vhs[1]);
        m_triangles.push_back(vhs[2]);

        m_lines.push_back(vhs[0]);
        m_lines.push_back(vhs[1]);
        m_lines.push_back(vhs[1]);
        m_lines.push_back(vhs[2]);
        m_lines.push_back(vhs[2]);
        m_lines.push_back(vhs[3]);

        if (hehs[3] != hehs[0])
        {
            m_triangles.push_back(vhs[2]);
            m_triangles.push_back(vhs[3]);
            m_triangles.push_back(vhs[0]);

            m_lines.push_back(vhs[3]);
            m_lines.push_back(vhs[0]);
        }

        bool boundary = false;

        for (size_t i = 0; i < 4; ++i)
        {
            if (m_table.deref(m_table.twin(hehs[i])).fh.index == heds::invalid)
            {
                boundary = true;

                m_holes.push_back(vhs[i]);
                m_holes.push_back(vhs[i == 3 ? 0 : i + 1]);
            }
        }

        if (boundary)
        {
            m_boundaries.push_back(vhs[0]);
            m_boundaries.push_back(vhs[1]);
            m_boundaries.push_back(vhs[2]);
            m_boundaries.push_back(vhs[2]);
            m_boundaries.push_back(vhs[3]);
            m_boundaries.push_back(vhs[0]);
        }
    }

    for (size_t i = 0; i < normalsMap.size(); ++i)
        m_vertices[i].normal = glm::normalize(normalsMap[i].first / normalsMap[i].second);
}

void Mesh::applyTransformation(heds::FaceHandle fh, const glm::mat4& trf)
{
    m_bufferSubData = true;
    m_subDataIndices.clear();

    glm::vec3 center{ 0 }; uint32_t vertices = 0;
    
    heds::HalfEdgeHandle start_heh = m_table.deref(fh).heh;
    heds::HalfEdgeHandle next_heh = start_heh;

    do
    {
        ++vertices;
        center += m_table.getEndPoint(next_heh);
        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);

    center /= static_cast<float>(vertices);

    do
    {
        heds::VertexHandle vh = m_table.destVertex(next_heh);

        glm::vec4 position = glm::vec4(m_table.getPoint(vh), 1.0f);
        position = glm::translate(-center) * position;
        position = trf * position;
        position = glm::translate(center) * position;

        m_table.setPoint(vh, position);

        m_bbox.min.x = std::min(m_bbox.min.x, position.x);
        m_bbox.min.y = std::min(m_bbox.min.y, position.y);
        m_bbox.min.z = std::min(m_bbox.min.z, position.z);

        m_bbox.max.x = std::max(m_bbox.max.x, position.x);
        m_bbox.max.y = std::max(m_bbox.max.y, position.y);
        m_bbox.max.z = std::max(m_bbox.max.z, position.z);

        m_vertices[vh].position = position;

        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);


    // Calculate normals
    std::set<heds::FaceHandle> affectedFaces;
    std::set<heds::VertexHandle> affectedVertices;

    do
    {
        heds::VertexHandle vh = m_table.destVertex(next_heh);

        heds::HalfEdgeHandle inner_start_heh = m_table.deref(vh).heh;
        heds::HalfEdgeHandle inner_next_heh = inner_start_heh;

        do
        {
            affectedFaces.insert(m_table.deref(inner_next_heh).fh);
            inner_next_heh = m_table.next(m_table.twin(inner_next_heh));

        } while (inner_next_heh != inner_start_heh);

        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);

    for (auto& fh : affectedFaces)
    {
        if (fh != heds::invalid)
        {
            heds::HalfEdgeHandle start_heh = m_table.deref(fh).heh;
            heds::HalfEdgeHandle next_heh = start_heh;

            do
            {
                affectedVertices.insert(m_table.destVertex(next_heh));
                next_heh = m_table.next(next_heh);

            } while (next_heh != start_heh);
        }
    }

    for (auto& vh : affectedVertices)
    {
        if (vh != heds::invalid)
        {
            std::set<heds::FaceHandle> adjacentFaces;

            heds::HalfEdgeHandle start_heh = m_table.deref(vh).heh;
            heds::HalfEdgeHandle next_heh = start_heh;

            do
            {
                adjacentFaces.insert(m_table.deref(next_heh).fh);
                next_heh = m_table.next(m_table.twin(next_heh));

            } while (next_heh != start_heh);

            glm::vec3 normalsSum(0.f);

            for (auto& fh : adjacentFaces)
            {
                if (fh != heds::invalid)
                {
                    heds::HalfEdgeHandle heh0 = m_table.deref(fh).heh;
                    heds::HalfEdgeHandle heh1 = m_table.next(heh0);
                    heds::HalfEdgeHandle heh2 = m_table.next(heh1);

                    glm::vec3 ab = m_vertices[m_table.sourceVertex(heh1)].position - m_vertices[m_table.sourceVertex(heh0)].position;
                    glm::vec3 bc = m_vertices[m_table.sourceVertex(heh2)].position - m_vertices[m_table.sourceVertex(heh1)].position;
                    normalsSum += glm::cross(ab, bc);
                }
            }

            m_vertices[vh].normal = glm::normalize(normalsSum / static_cast<float>(adjacentFaces.size()));
            m_subDataIndices.push_back(vh);
        }
    }
}

void Mesh::applyTransformation(heds::VertexHandle vh, const glm::mat4& trf)
{
    m_bufferSubData = true;
    m_subDataIndices.clear();
    
    glm::vec3 center = m_table.getPoint(vh);

    glm::vec4 position = glm::vec4(m_table.getPoint(vh), 1.0f);
    position = glm::translate(-center) * position;
    position = trf * position;
    position = glm::translate(center) * position;

    m_table.setPoint(vh, position);

    m_bbox.min.x = std::min(m_bbox.min.x, position.x);
    m_bbox.min.y = std::min(m_bbox.min.y, position.y);
    m_bbox.min.z = std::min(m_bbox.min.z, position.z);

    m_bbox.max.x = std::max(m_bbox.max.x, position.x);
    m_bbox.max.y = std::max(m_bbox.max.y, position.y);
    m_bbox.max.z = std::max(m_bbox.max.z, position.z);

    m_vertices[vh].position = position;
    
    // Calculate normals
    std::set<heds::FaceHandle> affectedFaces;
    std::set<heds::VertexHandle> affectedVertices;

    heds::HalfEdgeHandle start_heh = m_table.deref(vh).heh;
    heds::HalfEdgeHandle next_heh = start_heh;

    do
    {
        affectedFaces.insert(m_table.deref(next_heh).fh);
        next_heh = m_table.next(m_table.twin(next_heh));

    } while (next_heh != start_heh);

    for (auto& fh : affectedFaces)
    {
        if (fh != heds::invalid)
        {
            heds::HalfEdgeHandle start_heh = m_table.deref(fh).heh;
            heds::HalfEdgeHandle next_heh = start_heh;

            do
            {
                affectedVertices.insert(m_table.destVertex(next_heh));
                next_heh = m_table.next(next_heh);

            } while (next_heh != start_heh);
        }
    }

    for (auto& vh : affectedVertices)
    {
        if (vh != heds::invalid)
        {
            std::set<heds::FaceHandle> adjacentFaces;

            heds::HalfEdgeHandle start_heh = m_table.deref(vh).heh;
            heds::HalfEdgeHandle next_heh = start_heh;

            do
            {
                adjacentFaces.insert(m_table.deref(next_heh).fh);
                next_heh = m_table.next(m_table.twin(next_heh));

            } while (next_heh != start_heh);

            glm::vec3 normalsSum(0.f);

            for (auto& fh : adjacentFaces)
            {
                if (fh != heds::invalid)
                {
                    heds::HalfEdgeHandle heh0 = m_table.deref(fh).heh;
                    heds::HalfEdgeHandle heh1 = m_table.next(heh0);
                    heds::HalfEdgeHandle heh2 = m_table.next(heh1);

                    glm::vec3 ab = m_vertices[m_table.sourceVertex(heh1)].position - m_vertices[m_table.sourceVertex(heh0)].position;
                    glm::vec3 bc = m_vertices[m_table.sourceVertex(heh2)].position - m_vertices[m_table.sourceVertex(heh1)].position;
                    normalsSum += glm::cross(ab, bc);
                }
            }

            m_vertices[vh].normal = glm::normalize(normalsSum / static_cast<float>(adjacentFaces.size()));
            m_subDataIndices.push_back(vh);
        }
    }
}

void Mesh::deleteFace(heds::FaceHandle fh)
{
    m_table.deleteFace(fh);
    update();
}

const heds::HalfEdgeTable& Mesh::getHalfEdgeTable() const
{
    return m_table;
}

const BoundaryBox& Mesh::getBoundingBox()
{
    return m_bbox;
}

void Mesh::setName(const std::string& name)
{
    m_name = name;
}

void Mesh::setMaterial(const Material& material)
{
    m_material = material;
}

const std::string& Mesh::getName() const
{
    return m_name;
}

const Material& Mesh::getMaterial() const
{
    return m_material;
}