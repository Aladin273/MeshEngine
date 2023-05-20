#include "Mesh.h"

Mesh::Mesh(const Utils::HalfEdgeTable& halfEdgeTable)
    : m_table(halfEdgeTable)
{
    update();
}

Mesh::Mesh(const Utils::HalfEdgeTable& halfEdgeTable, const Material& material)
    : m_table(halfEdgeTable), m_material(material)
{
    update();
}

void Mesh::render(IRenderSystem& rs)
{
    if (m_drawTriangles)
    {
        rs.setupMaterial(m_material);
        rs.renderTriangles(m_triangles);
    }

    if (m_drawLines || m_drawHoles || m_drawBoundaries)
    {
        bool light = rs.getLighting();
        rs.setLighting(false);

        if (m_drawLines)
            rs.renderLines(m_lines);
        if (m_drawHoles)
            rs.renderLines(m_holes);
        if (m_drawBoundaries)
            rs.renderTriangles(m_boundaries);

        rs.setLighting(light);
    }
}

void Mesh::update()
{
    updateBbox();

    if (m_drawTriangles)
        updateTriangles();
    if (m_drawLines)
        updateLines();
    if (m_drawHoles)
        updateHoles();
    if (m_drawBoundaries)
        updateBoundaryFaces();
}

void Mesh::update(glm::vec3 orig, glm::vec3 target)
{
    if (target.x < m_bbox.min.x)
        m_bbox.min.x = target.x;
    if (target.y < m_bbox.min.y)
        m_bbox.min.y = target.y;
    if (target.z < m_bbox.min.z)
        m_bbox.min.z = target.z;

    if (target.x > m_bbox.max.x)
        m_bbox.max.x = target.x;
    if (target.y > m_bbox.max.y)
        m_bbox.max.y = target.y;
    if (target.z > m_bbox.max.z)
        m_bbox.max.z = target.z;

    if (m_drawTriangles)
    {
        for (auto& vertex : m_triangles)
        {
            if (vertex.position == orig)
                vertex.position = target;
        };
    }
    if (m_drawLines)
    {
        for (auto& vertex : m_lines)
        {
            if (vertex.position == orig)
                vertex.position = target;
        };
    }
    if (m_drawHoles)
    {
        for (auto& vertex : m_holes)
        {
            if (vertex.position == orig)
                vertex.position = target;
        };
    }
    if (m_drawBoundaries)
    {
        for (auto& vertex : m_boundaries)
        {
            if (vertex.position == orig)
                vertex.position = target;
        };
    }
}

void Mesh::updateBbox()
{
    if (!m_table.getVertices().empty())
    {
        m_bbox = { m_table.getVertices().front().data,  m_table.getVertices().front().data };

        for (auto& vertex : m_table.getVertices())
        {
            if (vertex.data.x < m_bbox.min.x)
                m_bbox.min.x = vertex.data.x;
            if (vertex.data.y < m_bbox.min.y)
                m_bbox.min.y = vertex.data.y;
            if (vertex.data.z < m_bbox.min.z)
                m_bbox.min.z = vertex.data.z;

            if (vertex.data.x > m_bbox.max.x)
                m_bbox.max.x = vertex.data.x;
            if (vertex.data.y > m_bbox.max.y)
                m_bbox.max.y = vertex.data.y;
            if (vertex.data.z > m_bbox.max.z)
                m_bbox.max.z = vertex.data.z;
        }
    }
}

void Mesh::updateLines()
{
    m_lines.clear();

    glm::vec3 normal{ 0.0f };
    std::set<Utils::HalfEdgeHandle> collection;
    std::set<Utils::HalfEdgeHandle>::iterator it;

    for (auto& halfedge : m_table.getHalfEdges())
    {
        it = collection.find(halfedge.twin);

        if (it == collection.end())
        {
            Utils::HalfEdgeHandle heh = m_table.handle(halfedge);

            collection.emplace(heh);

            const glm::vec3& a = m_table.getStartPoint(heh);
            const glm::vec3& b = m_table.getEndPoint(heh);

            m_lines.push_back({ a, normal, m_colorLines });
            m_lines.push_back({ b, normal, m_colorLines });
        }
    }
}

void Mesh::updateHoles()
{
    m_holes.clear();

    glm::vec3 normal{ 0.0f };
    std::set<Utils::HalfEdgeHandle> edges;

    for (auto& halfedge : m_table.getHalfEdges())
    {
        if (halfedge.fh.index == Utils::invalid && edges.find(m_table.handle(halfedge)) == edges.end())
        {
            Utils::HalfEdgeHandle start_heh = m_table.handle(halfedge);
            Utils::HalfEdgeHandle next_heh = start_heh;

            do
            {
                edges.emplace(next_heh);

                const glm::vec3& a = m_table.getStartPoint(next_heh);
                const glm::vec3& b = m_table.getEndPoint(next_heh);

                m_holes.push_back({ a, normal, m_colorHoles });
                m_holes.push_back({ b, normal, m_colorHoles });

                next_heh = m_table.next(next_heh);
            } while (next_heh != start_heh);
        }
    }
}

void Mesh::updateTriangles()
{
    m_triangles.clear();

    for (auto& face : m_table.getFaces())
    {
        Utils::HalfEdgeHandle heh0 = face.heh;
        Utils::HalfEdgeHandle heh1 = m_table.next(heh0);
        Utils::HalfEdgeHandle heh2 = m_table.next(heh1);
        Utils::HalfEdgeHandle heh3 = m_table.next(heh2);

        const glm::vec3& a = m_table.getEndPoint(heh0);
        const glm::vec3& b = m_table.getEndPoint(heh1);
        const glm::vec3& c = m_table.getEndPoint(heh2);

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - b));

        m_triangles.push_back({ a, normal, m_colorTriangles });
        m_triangles.push_back({ b, normal, m_colorTriangles });
        m_triangles.push_back({ c, normal, m_colorTriangles });

        if (heh3 != heh0) // if 4 vertices
        {
            const glm::vec3& d = m_table.getEndPoint(heh3);

            m_triangles.push_back({ c, normal, m_colorTriangles });
            m_triangles.push_back({ d, normal, m_colorTriangles });
            m_triangles.push_back({ a, normal, m_colorTriangles });
        }
    }
}

void Mesh::updateBoundaryFaces()
{
    m_boundaries.clear();

    glm::vec3 normal{ 0.0f };

    for (auto& face : m_table.getFaces())
    {
        Utils::HalfEdgeHandle heh0 = face.heh;
        Utils::HalfEdgeHandle heh1 = m_table.next(heh0);
        Utils::HalfEdgeHandle heh2 = m_table.next(heh1);
        Utils::HalfEdgeHandle heh3 = m_table.next(heh2);

        if ((m_table.deref(m_table.twin(heh0)).fh.index == Utils::invalid) ||
            (m_table.deref(m_table.twin(heh1)).fh.index == Utils::invalid) ||
            (m_table.deref(m_table.twin(heh2)).fh.index == Utils::invalid) ||
            (m_table.deref(m_table.twin(heh3)).fh.index == Utils::invalid))
        {
            const glm::vec3& a = m_table.getEndPoint(heh0);
            const glm::vec3& b = m_table.getEndPoint(heh1);
            const glm::vec3& c = m_table.getEndPoint(heh2);

            m_boundaries.push_back({ a, normal, m_colorBoundaries });
            m_boundaries.push_back({ b, normal, m_colorBoundaries });
            m_boundaries.push_back({ c, normal, m_colorBoundaries });

            if (heh3 != heh0)
            {
                const glm::vec3& d = m_table.getEndPoint(heh3);

                m_boundaries.push_back({ c, normal, m_colorBoundaries });
                m_boundaries.push_back({ d, normal, m_colorBoundaries });
                m_boundaries.push_back({ a, normal, m_colorBoundaries });
            }
        }
    }
}

void Mesh::applyTransformation(Utils::VertexHandle vh, const glm::mat4& trf)
{
    glm::vec3 center = m_table.getPoint(vh);

    m_table.setPoint(vh, glm::translate(-center) * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, trf * glm::vec4(m_table.getPoint(vh), 1.0f));
    m_table.setPoint(vh, glm::translate(center) * glm::vec4(m_table.getPoint(vh), 1.0f));

    update(center, m_table.getPoint(vh));
}

void Mesh::applyTransformation(Utils::FaceHandle fh, const glm::mat4& trf)
{
    Utils::HalfEdgeHandle start_heh = m_table.deref(fh).heh;
    Utils::HalfEdgeHandle next_heh = start_heh;
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
        
        update(vertex, m_table.getEndPoint(next_heh));

        next_heh = m_table.next(next_heh);
    } 
    while (next_heh != start_heh);
}

void Mesh::deleteFace(Utils::FaceHandle fh)
{
    m_table.deleteFace(fh);
    update();
}

const Utils::HalfEdgeTable& Mesh::getHalfEdgeTable() const
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

void Mesh::setDrawBoundaries(bool enable)
{
    m_drawBoundaries = enable;
}

void Mesh::setDrawTriangles(bool enable)
{
    m_drawTriangles = enable;
}

void Mesh::setDrawLines(bool enable)
{
    m_drawLines = enable;
}

void Mesh::setDrawHoles(bool enable)
{
    m_drawHoles = enable;
}

bool Mesh::getDrawBoundaries() const
{
    return m_drawBoundaries;
}

bool Mesh::getDrawTriangles() const
{
    return m_drawTriangles;
}

bool Mesh::getDrawLines() const
{
    return m_drawLines;
}

bool Mesh::getDrawHoles() const
{
    return m_drawHoles;
}

void Mesh::setColorBoundaries(glm::vec4 color)
{
    m_colorBoundaries = color;
}

void Mesh::setColorTriangles(glm::vec4 color)
{
    m_colorTriangles = color;
}

void Mesh::setColorLines(glm::vec4 color)
{
    m_colorLines = color;
}

void Mesh::setColorHoles(glm::vec4 color)
{
    m_colorHoles = color;
}

const glm::vec4& Mesh::getColorBoundaries()
{
    return m_colorBoundaries;
}

const glm::vec4& Mesh::getColorTriangles()
{
    return m_colorTriangles;
}

const glm::vec4& Mesh::getColorLines()
{
    return m_colorLines;
}

const glm::vec4& Mesh::getColorHoles()
{
    return m_colorHoles;
}
