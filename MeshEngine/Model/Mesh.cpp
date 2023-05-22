#include "Mesh.h"

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

void Mesh::render(IRenderSystem& rs)
{
    if (renderTriangles)
    {
        rs.setupMaterial(m_material);
        rs.renderTriangles(m_triangles, colorTriangles);
    }

    bool light = rs.getLighting();
    rs.setLighting(false);

    if (renderLines)
        rs.renderLines(m_lines, colorLines);
    if (renderHoles)
        rs.renderLines(m_holes, colorHoles);
    if (renderBoundaries)
        rs.renderTriangles(m_boundaries, colorBoundaries);

    rs.setLighting(light);
}

void Mesh::update()
{    
    if (!m_table.getVertices().empty())
        m_bbox = { m_table.getVertices().front().data,  m_table.getVertices().front().data };

    m_triangles.clear();
    m_lines.clear();
    m_holes.clear();
    m_boundaries.clear();

    std::vector<heds::HalfEdgeHandle> hehs(4);
    std::vector<glm::vec3> vertices(4);

    for (auto& face : m_table.getFaces())
    {
        for (size_t i = 0; i < 4; ++i)
            hehs[i] = i == 0 ? face.heh : m_table.next(hehs[i-1]);

        for (size_t i = 0; i < 4; ++i)
            vertices[i] = m_table.getStartPoint(hehs[i]);

        for (const auto& vertex : vertices)
        {
            if (vertex.x < m_bbox.min.x)
                m_bbox.min.x = vertex.x;
            if (vertex.y < m_bbox.min.y)
                m_bbox.min.y = vertex.y;
            if (vertex.z < m_bbox.min.z)
                m_bbox.min.z = vertex.z;

            if (vertex.x > m_bbox.max.x)
                m_bbox.max.x = vertex.x;
            if (vertex.y > m_bbox.max.y)
                m_bbox.max.y = vertex.y;
            if (vertex.z > m_bbox.max.z)
                m_bbox.max.z = vertex.z;
        }

        glm::vec3 normal = glm::normalize(glm::cross(vertices[1] - vertices[0], vertices[2] - vertices[1]));

        m_triangles.push_back({ vertices[0], normal, colorTriangles });
        m_triangles.push_back({ vertices[1], normal, colorTriangles });
        m_triangles.push_back({ vertices[2], normal, colorTriangles });

        m_lines.push_back({ vertices[0], normal, colorLines });
        m_lines.push_back({ vertices[1], normal, colorLines });
        m_lines.push_back({ vertices[1], normal, colorLines });
        m_lines.push_back({ vertices[2], normal, colorLines });
        m_lines.push_back({ vertices[2], normal, colorLines });
        m_lines.push_back({ vertices[3], normal, colorLines });

        if (hehs[3] != hehs[0])
        {
            m_triangles.push_back({ vertices[2], normal, colorTriangles });
            m_triangles.push_back({ vertices[3], normal, colorTriangles });
            m_triangles.push_back({ vertices[0], normal, colorTriangles });

            m_lines.push_back({ vertices[3], normal, colorLines });
            m_lines.push_back({ vertices[0], normal, colorLines });
        }

        bool boundary = false;

        for (size_t i = 0; i < 4; ++i)
        {
            if (m_table.deref(m_table.twin(hehs[i])).fh.index == heds::invalid)
            {
                m_holes.push_back({ vertices[i], normal, colorHoles });
                m_holes.push_back({ vertices[i == 3 ? 0 : i + 1], normal, colorHoles });
                boundary = true;
            }
        }

        if (boundary)
        {
            m_boundaries.push_back({ vertices[0], normal, colorBoundaries });
            m_boundaries.push_back({ vertices[1], normal, colorBoundaries });
            m_boundaries.push_back({ vertices[2], normal, colorBoundaries });
            m_boundaries.push_back({ vertices[2], normal, colorBoundaries });
            m_boundaries.push_back({ vertices[3], normal, colorBoundaries });
            m_boundaries.push_back({ vertices[0], normal, colorBoundaries });
        }
    }
}

void Mesh::applyTransformation(heds::FaceHandle fh, const glm::mat4& trf)
{
    heds::HalfEdgeHandle start_heh = m_table.deref(fh).heh;
    heds::HalfEdgeHandle next_heh = start_heh;
    glm::vec3 center{ 0 }; float vertices = 0;

    do
    {
        center += m_table.getEndPoint(next_heh);
        next_heh = m_table.next(next_heh);
        ++vertices;
    } while (next_heh != start_heh);

    center /= vertices;

    do
    {
        glm::vec3 vertex = m_table.getEndPoint(next_heh);

        m_table.setEndPoint(next_heh, glm::translate(-center) * glm::vec4(m_table.getEndPoint(next_heh), 1.0f));
        m_table.setEndPoint(next_heh, trf * glm::vec4(m_table.getEndPoint(next_heh), 1.0f));
        m_table.setEndPoint(next_heh, glm::translate(center) * glm::vec4(m_table.getEndPoint(next_heh), 1.0f));

        next_heh = m_table.next(next_heh);
    } while (next_heh != start_heh);

    update();
}

void Mesh::applyTransformation(heds::VertexHandle vh, const glm::mat4& trf)
{
    glm::vec3 center = m_table.getPoint(vh);

    m_table.setPoint(vh, glm::translate(-center) * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, trf * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, glm::translate(center) * glm::vec4(m_table.getPoint(vh), 1.0f));

    update();
}

void Mesh::deleteFace(heds::FaceHandle fh)
{
    m_table.deleteFace(fh);
    update();
}

void Mesh::deleteVertex(heds::VertexHandle fh)
{
    //m_table.deleteFace(fh);
    //update();
}

const heds::HalfEdgeTable& Mesh::getHalfEdgeTable() const
{
    return m_table;
}

const bbox& Mesh::getBoundingBox()
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