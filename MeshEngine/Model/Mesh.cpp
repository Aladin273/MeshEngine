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
        rs.renderTriangles(m_vertices, m_trianglesIndices, colorTriangles);
    }

    bool light = rs.getLighting();
    rs.setLighting(false);

    if (renderLines)
        rs.renderLines(m_vertices, m_linesIndices, colorLines);
    if (renderHoles)
        rs.renderLines(m_vertices, m_holesIndices, colorHoles);
    if (renderBoundaries)
        rs.renderTriangles(m_vertices, m_boundariesIndices, colorBoundaries);

    rs.setLighting(light);
}

void Mesh::update()
{
    if (!m_table.getVertices().empty())
        m_bbox = { m_table.getVertices().front().data,  m_table.getVertices().front().data };

    m_vertices.clear();
    m_trianglesIndices.clear();
    m_linesIndices.clear();
    m_holesIndices.clear();
    m_boundariesIndices.clear();
    m_bboxDataIndices.clear();

    for (const auto& vertex : m_table.getVertices())
        m_vertices.push_back({ vertex.data, {}, {} });

    for (const auto& vertex : m_vertices)
    {
        if (vertex.position.x < m_bbox.min.x)
            m_bbox.min.x = vertex.position.x;
        if (vertex.position.y < m_bbox.min.y)
            m_bbox.min.y = vertex.position.y;
        if (vertex.position.z < m_bbox.min.z)
            m_bbox.min.z = vertex.position.z;

        if (vertex.position.x > m_bbox.max.x)
            m_bbox.max.x = vertex.position.x;
        if (vertex.position.y > m_bbox.max.y)
            m_bbox.max.y = vertex.position.y;
        if (vertex.position.z > m_bbox.max.z)
            m_bbox.max.z = vertex.position.z;
    }

    std::vector<heds::HalfEdgeHandle> hehs(4);
    std::vector<heds::VertexHandle> vertices(4);

    for (auto& face : m_table.getFaces())
    {
        for (size_t i = 0; i < 4; ++i)
            hehs[i] = i == 0 ? face.heh : m_table.next(hehs[i - 1]);

        for (size_t i = 0; i < 4; ++i)
            vertices[i] = m_table.sourceVertex(hehs[i]);

        glm::vec3 ab = glm::normalize(m_vertices[vertices[1].index].position - m_vertices[vertices[0].index].position);
        glm::vec3 bc = glm::normalize(m_vertices[vertices[2].index].position - m_vertices[vertices[1].index].position);
        glm::vec3 normal = glm::normalize(glm::cross(ab, bc));

        for (size_t i = 0; i < 4; ++i)
            //m_vertices[vertices[i].index].normal = normal;
            m_vertices[vertices[i].index].normal = m_vertices[vertices[i].index].position;

        m_trianglesIndices.push_back(vertices[0].index);
        m_trianglesIndices.push_back(vertices[1].index);
        m_trianglesIndices.push_back(vertices[2].index);

        m_linesIndices.push_back(vertices[0].index);
        m_linesIndices.push_back(vertices[1].index);
        m_linesIndices.push_back(vertices[1].index);
        m_linesIndices.push_back(vertices[2].index);
        m_linesIndices.push_back(vertices[2].index);
        m_linesIndices.push_back(vertices[3].index);

        if (hehs[3] != hehs[0])
        {
            m_trianglesIndices.push_back(vertices[2].index);
            m_trianglesIndices.push_back(vertices[3].index);
            m_trianglesIndices.push_back(vertices[0].index);

            m_linesIndices.push_back(vertices[3].index);
            m_linesIndices.push_back(vertices[0].index);
        }

        bool boundary = false;

        for (size_t i = 0; i < 4; ++i)
        {
            if (m_table.deref(m_table.twin(hehs[i])).fh.index == heds::invalid)
            {
                m_holesIndices.push_back(vertices[i].index);
                m_holesIndices.push_back(vertices[i == 3 ? 0 : i + 1].index);
                boundary = true;
            }
        }

        if (boundary)
        {
            m_boundariesIndices.push_back(vertices[0].index);
            m_boundariesIndices.push_back(vertices[1].index);
            m_boundariesIndices.push_back(vertices[2].index);
            m_boundariesIndices.push_back(vertices[2].index);
            m_boundariesIndices.push_back(vertices[3].index);
            m_boundariesIndices.push_back(vertices[0].index);
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

        m_vertices[m_table.destVertex(next_heh).index].position = m_table.getEndPoint(next_heh);
        m_vertices[m_table.destVertex(next_heh).index].normal = trf * glm::vec4(m_vertices[m_table.destVertex(next_heh).index].normal, 0.0f);

        next_heh = m_table.next(next_heh);
    } while (next_heh != start_heh);
}

void Mesh::applyTransformation(heds::VertexHandle vh, const glm::mat4& trf)
{
    glm::vec3 center = m_table.getPoint(vh);

    m_table.setPoint(vh, glm::translate(-center) * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, trf * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, glm::translate(center) * glm::vec4(m_table.getPoint(vh), 1.0f));

    m_vertices[vh.index].position = m_table.getPoint(vh);
    m_vertices[vh.index].normal = trf * glm::vec4(m_vertices[vh.index].normal, 0.0f);
}

void Mesh::deleteFace(heds::FaceHandle fh)
{
    m_table.deleteFace(fh);
    update();
}

void Mesh::deleteVertex(heds::VertexHandle fh)
{
    //m_table.deleteFace(fh);
    update();
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