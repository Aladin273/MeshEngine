#include "HalfEdge.h"

using namespace Utils;

/////////////////////// Actions

VertexHandle HalfEdgeTable::addVertex(glm::vec3 position) // std::find, if exists, return handle of existing?
{
    VertexHandle vh{ static_cast<int64_t>(m_vertices.size()) };
    m_vertices.push_back(Vertex{ {invalid}, position });

    return vh;
}

FaceHandle HalfEdgeTable::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2)
{
    if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid)
        throw std::exception("Invalid VertexHandle");

    deleteFakeTwins();

    int64_t size = static_cast<int64_t>(m_halfEdges.size());
    FaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
    HalfEdgeHandle twin{ invalid };
    HalfEdgeHandle heh0{ size };
    HalfEdgeHandle heh1{ size + 1 };
    HalfEdgeHandle heh2{ size + 2 };

    deref(vh0).heh = heh0;
    deref(vh1).heh = heh1;
    deref(vh2).heh = heh2;

    m_halfEdges.push_back(HalfEdge{ fh, vh1, twin, heh1, heh2 });
    m_halfEdges.push_back(HalfEdge{ fh, vh2, twin, heh2, heh0 });
    m_halfEdges.push_back(HalfEdge{ fh, vh0, twin, heh0, heh1 });
    m_faces.push_back({ heh0 });

    return fh;
}

FaceHandle HalfEdgeTable::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3)
{
    if (vh0.index <= invalid || vh1.index <= invalid || vh2.index <= invalid || vh3.index <= invalid)
        throw std::exception("Invalid VertexHandle");

    deleteFakeTwins();

    int64_t size = static_cast<int64_t>(m_halfEdges.size());
    FaceHandle fh{ static_cast<int64_t>(m_faces.size()) };
    HalfEdgeHandle twin{ invalid };
    HalfEdgeHandle heh0{ size };
    HalfEdgeHandle heh1{ size + 1 };
    HalfEdgeHandle heh2{ size + 2 };
    HalfEdgeHandle heh3{ size + 3 };

    deref(vh0).heh = heh0;
    deref(vh1).heh = heh1;
    deref(vh2).heh = heh2;
    deref(vh3).heh = heh3;

    m_halfEdges.push_back(HalfEdge{ fh, vh1, twin, heh1, heh3 });
    m_halfEdges.push_back(HalfEdge{ fh, vh2, twin, heh2, heh0 });
    m_halfEdges.push_back(HalfEdge{ fh, vh3, twin, heh3, heh1 });
    m_halfEdges.push_back(HalfEdge{ fh, vh0, twin, heh0, heh2 });
    m_faces.push_back({ heh0 });

    return fh;
}

void HalfEdgeTable::connectTwins()
{
    deleteFakeTwins();

    HalfEdgeHandle heh_empty{ invalid };
    std::vector<HalfEdgeHandle> notwins;
    std::map<std::pair<VertexHandle, VertexHandle>, HalfEdgeHandle> collection;
    std::map<std::pair<VertexHandle, VertexHandle>, HalfEdgeHandle>::iterator it;

    // Prepare data
    for (auto& he : m_halfEdges)
    {
        if (he.twin == heh_empty)
            collection.emplace(std::pair<VertexHandle, VertexHandle>(he.dst, deref(he.prev).dst), handle(he));
    }

    // Connecting twins
    for (auto& he : m_halfEdges)
    {
        it = collection.find(std::pair<VertexHandle, VertexHandle>(deref(he.prev).dst, he.dst));

        if (it != collection.end())
        {
            he.twin = it->second;
            deref(it->second).twin = handle(he);
        }
        else
            notwins.push_back(handle(he));
    }

    m_fakeTwins = m_halfEdges.size();

    // Building fake twins
    for (auto& boundary : notwins)
    {
        if (deref(boundary).twin != heh_empty)
            continue;

        int64_t check1 = invalid;
        int64_t check2 = invalid;
        int64_t added = m_halfEdges.size();
        FaceHandle fh_empty{};
        HalfEdgeHandle start_heh = boundary;
        HalfEdgeHandle previus = prev(boundary);

        deref(boundary).twin.index = added;  // Add twin to boundary
        m_halfEdges.push_back({ fh_empty, destVertex(previus), boundary, added + 1, added - 1 });

        while ((destVertex(previus) != destVertex(start_heh)) && (check1 < m_fakeTwins)) // visiting all the halfedges directed to a specified Vertex
        {   // Ideally, the first condition should always be met, 
            // but if the model is non-valid, then something can go wrong, so I introduced a spare
            ++check1;
            added = m_halfEdges.size();

            while ((twin(previus) != heh_empty) && (check2 < m_fakeTwins)) // find while prev not boundary
            {
                ++check2;
                boundary = twin(previus);
                previus = prev(boundary);
            }
            check2 = invalid;

            deref(previus).twin.index = added;
            m_halfEdges.push_back({ fh_empty, destVertex(prev(previus)), previus, added + 1, added - 1 });

            boundary = previus;
            previus = prev(boundary);
        }
        check1 = invalid;

        deref(HalfEdgeHandle{ added }).next = twin(start_heh); // Connect start with end
        deref(twin(start_heh)).prev.index = added;
    }
}

void HalfEdgeTable::deleteFakeTwins()
{
    if (m_fakeTwins != invalid) // there is no need to do anything
    {
        for (size_t i = m_fakeTwins; i < m_halfEdges.size(); ++m_fakeTwins)
            deref(m_halfEdges[i].twin).twin = { invalid };

        m_halfEdges.erase(m_halfEdges.begin() + static_cast<int64_t>(m_fakeTwins), m_halfEdges.end());
        m_fakeTwins = invalid;
    }
}

void HalfEdgeTable::deleteFace(FaceHandle fh)
{
    if (fh.index >= static_cast<int64_t>(m_faces.size()) || fh.index <= invalid)
        throw std::exception("Invalid FaceHandle");

    HalfEdgeHandle start_heh = deref(fh).heh;
    HalfEdgeHandle next_heh = start_heh;

    std::vector<VertexHandle> temp_vertices;
    std::vector<HalfEdgeHandle> temp_halfEdges;

    std::vector<VertexHandle> marked_vertices;
    std::vector<HalfEdgeHandle> marked_halfEdges;

    std::vector<HalfEdgeHandle> mapping_halfEdges(m_halfEdges.size());
    std::vector<VertexHandle> mapping_vertices(m_vertices.size());

    // For all halfEdge heh of face fh do
    do
    {
        deref(next_heh).fh.index = invalid;
        temp_vertices.push_back(destVertex(next_heh));

        if (deref(twin(next_heh)).fh.index == invalid)
            temp_halfEdges.push_back(next_heh);

        next_heh = next(next_heh);
    } while (next_heh != start_heh);

    // Fix (heh)
    for (auto& he : temp_halfEdges)
    {
        HalfEdgeHandle& h0 = he;
        VertexHandle& v0 = deref(h0).dst;
        HalfEdgeHandle& next0 = deref(h0).next;
        HalfEdgeHandle& prev0 = deref(h0).prev;

        HalfEdgeHandle& h1 = deref(he).twin;
        VertexHandle& v1 = deref(h1).dst;
        HalfEdgeHandle& next1 = deref(h1).next;
        HalfEdgeHandle& prev1 = deref(h1).prev;

        deref(prev0).next = next1;
        deref(prev1).next = next0;

        deref(next1).prev = prev0;
        deref(next0).prev = prev1;

        // h0 mark as deleted
        // h1 mark as deleted
        marked_halfEdges.push_back(h0);
        marked_halfEdges.push_back(h1);

        if (deref(v0).heh == h1)
        {
            if (next0 == h1)
            {
                // v0 mark as deleted
                marked_vertices.push_back(v0);

                v0.index = invalid;
            }
            else
                deref(v0).heh = next0;
        }

        if (deref(v1).heh == h0)
        {
            if (next1 == h0)
            {
                // v1 mark as deleted
                marked_vertices.push_back(v1);

                v1.index = invalid;
            }
            else
                deref(v1).heh = next1;
        }
    }

    // Adjust outgoing halfEdge(VertexHandle vh)
    for (auto& vh : temp_vertices)
    {
        HalfEdgeHandle start_heh = deref(vh).heh;
        HalfEdgeHandle next_heh = start_heh;

        do
        {
            next_heh = next(twin(next_heh));
        } while ((deref(next_heh).fh.index == invalid) && (deref(twin(next_heh)).fh.index == invalid) && (next_heh != start_heh));

        deref(vh).heh = next_heh;
    }

    // Store mapping for halfEdges
    for (int index = 0; index < mapping_halfEdges.size(); ++index)
        mapping_halfEdges[index] = (HalfEdgeHandle{ index });

    // Store mapping for vertices
    for (int index = 0; index < mapping_vertices.size(); ++index)
        mapping_vertices[index] = (VertexHandle{ index });

    // Remove marked faces
    m_faces.erase(m_faces.begin() + fh.index);

    // Remove marked halfEdges
    for (auto& heh : marked_halfEdges)
    {
        m_halfEdges.erase(m_halfEdges.begin() + mapping_halfEdges[heh.index].index);

        // Update  mapping for halfEdges
        for (int64_t index = heh.index; index < mapping_halfEdges.size(); ++index)
            --mapping_halfEdges[index].index;
    }

    // Remove marked vertices
    for (auto& vh : marked_vertices)
    {
        m_vertices.erase(m_vertices.begin() + mapping_vertices[vh.index].index);

        // Update mapping for vertices
        for (int64_t index = vh.index; index < mapping_vertices.size(); ++index)
            --mapping_vertices[index].index;
    }

    // Remapping
    for (auto& halfedge : m_halfEdges)
    {
        if (halfedge.fh > fh)
            --halfedge.fh.index;

        halfedge.dst = mapping_vertices[halfedge.dst.index];
        halfedge.next = mapping_halfEdges[halfedge.next.index];
        halfedge.prev = mapping_halfEdges[halfedge.prev.index];
        halfedge.twin = mapping_halfEdges[halfedge.twin.index];
    }
    for (auto& vertex : m_vertices)
        vertex.heh = mapping_halfEdges[vertex.heh.index];
    for (auto& face : m_faces)
        face.heh = mapping_halfEdges[face.heh.index];
}

/////////////////////// Helpers

HalfEdgeHandle HalfEdgeTable::prev(HalfEdgeHandle heh) const
{
    return deref(heh).prev;
}

HalfEdgeHandle HalfEdgeTable::next(HalfEdgeHandle heh) const
{
    return deref(heh).next;
}

HalfEdgeHandle HalfEdgeTable::twin(HalfEdgeHandle heh) const
{
    return deref(heh).twin;
}

VertexHandle HalfEdgeTable::destVertex(HalfEdgeHandle heh) const
{
    return deref(heh).dst;
}

VertexHandle HalfEdgeTable::sourceVertex(HalfEdgeHandle heh) const
{
    return destVertex(twin(heh));
}

HalfEdge& HalfEdgeTable::deref(HalfEdgeHandle heh)
{
    return m_halfEdges[heh.index];
}

const HalfEdge& HalfEdgeTable::deref(HalfEdgeHandle heh) const
{
    return m_halfEdges[heh.index];
}

HalfEdgeHandle HalfEdgeTable::handle(const HalfEdge& he) const
{
    return { static_cast<int64_t>(&he - &m_halfEdges[0]) };
}

Vertex& HalfEdgeTable::deref(VertexHandle vh)
{
    return m_vertices[vh.index];
}

const Vertex& HalfEdgeTable::deref(VertexHandle vh) const
{
    return m_vertices[vh.index];
}

VertexHandle HalfEdgeTable::handle(const Vertex& v) const
{
    return { static_cast<int>(&v - &m_vertices[0]) };
}

Face& HalfEdgeTable::deref(FaceHandle fh)
{
    return m_faces[fh.index];
}

const Face& HalfEdgeTable::deref(FaceHandle fh) const
{
    return m_faces[fh.index];
}

FaceHandle HalfEdgeTable::handle(const Face& f) const
{
    return { static_cast<int>(&f - &m_faces[0]) };
}

/////////////////////// Data implementation 

const glm::vec3& HalfEdgeTable::getPoint(VertexHandle handle) const
{
    return m_vertices[handle.index].data;
}

void HalfEdgeTable::setPoint(VertexHandle handle, glm::vec3 point)
{
    m_vertices[handle.index].data = point;
}

const glm::vec3& HalfEdgeTable::getStartPoint(HalfEdgeHandle handle) const
{
    return m_vertices[sourceVertex(handle).index].data;
}

void HalfEdgeTable::setStartPoint(HalfEdgeHandle handle, glm::vec3 point)
{
    m_vertices[sourceVertex(handle).index].data = point;
}

const glm::vec3& HalfEdgeTable::getEndPoint(HalfEdgeHandle handle) const
{
    return m_vertices[destVertex(handle).index].data;
}

void HalfEdgeTable::setEndPoint(HalfEdgeHandle handle, glm::vec3 point)
{
    m_vertices[destVertex(handle).index].data = point;
}

/////////////////////// Containers getters

const std::vector<Vertex>& HalfEdgeTable::getVertices() const
{
    return m_vertices;
}

const std::vector<Face>& HalfEdgeTable::getFaces() const
{
    return m_faces;
}

const std::vector<HalfEdge>& HalfEdgeTable::getHalfEdges() const
{
    return m_halfEdges;
}

HalfEdgeTable Utils::createCube(glm::vec3 center, float length)
{
    Utils::HalfEdgeTable halfEdgeTable;
    float halfLength = length / 2;

    Utils::VertexHandle vh0 = halfEdgeTable.addVertex(glm::vec3{  halfLength,  halfLength, -halfLength } + center);
    Utils::VertexHandle vh1 = halfEdgeTable.addVertex(glm::vec3{  halfLength, -halfLength, -halfLength } + center);
    Utils::VertexHandle vh2 = halfEdgeTable.addVertex(glm::vec3{ -halfLength, -halfLength, -halfLength } + center);
    Utils::VertexHandle vh3 = halfEdgeTable.addVertex(glm::vec3{ -halfLength,  halfLength, -halfLength } + center);
    Utils::VertexHandle vh4 = halfEdgeTable.addVertex(glm::vec3{  halfLength,  halfLength,  halfLength } + center);
    Utils::VertexHandle vh5 = halfEdgeTable.addVertex(glm::vec3{  halfLength, -halfLength,  halfLength } + center);
    Utils::VertexHandle vh6 = halfEdgeTable.addVertex(glm::vec3{ -halfLength, -halfLength,  halfLength } + center);
    Utils::VertexHandle vh7 = halfEdgeTable.addVertex(glm::vec3{ -halfLength,  halfLength,  halfLength } + center);

    halfEdgeTable.addFace(vh0, vh1, vh2, vh3);
    halfEdgeTable.addFace(vh4, vh7, vh6, vh5);
    halfEdgeTable.addFace(vh0, vh4, vh5, vh1);
    halfEdgeTable.addFace(vh1, vh5, vh6, vh2);
    halfEdgeTable.addFace(vh2, vh6, vh7, vh3);
    halfEdgeTable.addFace(vh4, vh0, vh3, vh7);

    halfEdgeTable.connectTwins();

    return halfEdgeTable;
}

HalfEdgeTable Utils::createCylinder(glm::vec3 inDir, float R, float h, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f, cz = 0.0f, radius = R; int segments = numSubdivisions;

    std::vector<Utils::VertexHandle> vertices;
    Utils::HalfEdgeTable table;

    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(segments);

        float x = radius * glm::cos(theta);
        float y = radius * glm::sin(theta);

        vertices.push_back(table.addVertex({ x,y, h }));
        vertices.push_back(table.addVertex({ x,y, cz }));
    }

    vertices.push_back(vertices[0]);
    vertices.push_back(vertices[1]);
    vertices.push_back(table.addVertex({ cx, cy, h }));
    vertices.push_back(table.addVertex({ cx, cy, cz }));

    for (size_t i = 0; i + 4 < vertices.size(); i += 2)
    {
        table.addFace(vertices[i], vertices[i + 1], vertices[i + 2]);
        table.addFace(vertices[i + 2], vertices[i + 1], vertices[i + 3]);

        table.addFace(vertices[vertices.size() - 2], vertices[i], vertices[i + 2]);
        table.addFace(vertices[vertices.size() - 1], vertices[i + 3], vertices[i + 1]);
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.m_vertices)
            vertex.data = mat * glm::vec4(vertex.data, 1.0f);
    }

    table.connectTwins();

    return table;
}

HalfEdgeTable Utils::createCone(glm::vec3 inDir, float R, float h, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f, cz = 0.0f, radius = R; int segments = numSubdivisions;

    std::vector<Utils::VertexHandle> vertices;
    Utils::HalfEdgeTable table;

    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(segments);

        float x = radius * glm::cos(theta);
        float y = radius * glm::sin(theta);

        vertices.push_back(table.addVertex({ x, y, cz }));
    }

    vertices.push_back(vertices[0]);
    vertices.push_back(table.addVertex({ cx, cy, h }));
    vertices.push_back(table.addVertex({ cx, cy, cz }));

    for (size_t i = 0; i + 1 < vertices.size(); ++i)
    {
        table.addFace(vertices[vertices.size() - 2], vertices[i], vertices[i + 1]);
        table.addFace(vertices[vertices.size() - 1], vertices[i + 1], vertices[i]);
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.m_vertices)
            vertex.data = mat * glm::vec4(vertex.data, 1.0f);
    }

    table.connectTwins();

    return table;
}

HalfEdgeTable Utils::createTorus(glm::vec3 inDir, float minorRadius, float majorRadius, uint32_t majorSegments)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f; int segments = static_cast<int>(majorSegments);

    std::vector<Utils::VertexHandle> vertices;
    Utils::HalfEdgeTable table;

    auto mainSegmentAngleStep = glm::radians(360.0f / segments);
    auto tubeSegmentAngleStep = glm::radians(360.0f / segments);

    auto currentMainSegmentAngle = 0.0f;
    for (int i = 0, duplicate = 0; i < segments; ++i, duplicate += segments + 1)
    {
        // Calculate sine and cosine of main segment angle
        auto sinMainSegment = sin(currentMainSegmentAngle);
        auto cosMainSegment = cos(currentMainSegmentAngle);
        auto currentTubeSegmentAngle = 0.0f;
        for (int j = 0; j < segments; ++j)
        {
            // Calculate sine and cosine of tube segment angle
            auto sinTubeSegment = sin(currentTubeSegmentAngle);
            auto cosTubeSegment = cos(currentTubeSegmentAngle);

            // Calculate vertex position on the surface of torus
            auto surfacePosition = glm::vec3(
                (majorRadius + minorRadius * cosTubeSegment) * cosMainSegment,
                (majorRadius + minorRadius * cosTubeSegment) * sinMainSegment,
                minorRadius * sinTubeSegment);

            vertices.push_back(table.addVertex(surfacePosition));

            // Update current tube angle
            currentTubeSegmentAngle += tubeSegmentAngleStep;
        }

        // Add first vertex from minor
        vertices.push_back(vertices[duplicate]);

        // Update main segment angle
        currentMainSegmentAngle += mainSegmentAngleStep;
    }

    // Due to duplicates change the value
    ++segments;

    // Add first minor
    for (int i = 0; i < segments; ++i)
    {
        vertices.push_back(vertices[i]);;
    }

    // Triangulation
    for (int i = 0; i < vertices.size() - segments; i += segments)
    {
        for (int a = i, b = i + 1, c = a + segments, d = b + segments; b < segments + i; ++a, ++b, ++c, ++d)
        {
            table.addFace(vertices[c], vertices[b], vertices[a]);
            table.addFace(vertices[b], vertices[c], vertices[d]);
        }
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.m_vertices)
            vertex.data = mat * glm::vec4(vertex.data, 1.0f);
    }

    table.connectTwins();

    return table;
}

HalfEdgeTable Utils::createArrow(glm::vec3 inDir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions)
{
    auto table1 = Utils::createCone(inDir, R1, h1, numSubdivisions);
    auto table2 = Utils::createCylinder(inDir, R2, h2, numSubdivisions);

    glm::vec3 delta = inDir * h2;

    for (auto& face : table1.getFaces())
    {
        Utils::HalfEdgeHandle heh0 = face.heh;
        Utils::HalfEdgeHandle heh1 = table1.next(heh0);
        Utils::HalfEdgeHandle heh2 = table1.next(heh1);

        glm::vec3 vec0{ table1.getEndPoint(heh0) }; vec0 += delta;
        glm::vec3 vec1{ table1.getEndPoint(heh1) }; vec1 += delta;
        glm::vec3 vec2{ table1.getEndPoint(heh2) }; vec2 += delta;

        Utils::VertexHandle v0 = table2.addVertex(vec0);
        Utils::VertexHandle v1 = table2.addVertex(vec1);
        Utils::VertexHandle v2 = table2.addVertex(vec2);

        table2.addFace(v0, v1, v2);
    }

    table2.connectTwins();

    return table2;
}

HalfEdgeTable Utils::createPlane(glm::vec3 inDir, float width, float heigth, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    int segments = glm::sqrt(numSubdivisions);
    float stepW = width / segments;
    float stepH = heigth / segments;
    float halfWidth = width / 2;
    float halfHeight = heigth / 2;
    float cx = 0.0f, cy = 0.0f, cz = 0.0f; 

    std::vector<std::vector<Utils::VertexHandle>> vertices;
    Utils::HalfEdgeTable table;

    for (float i = -halfWidth; i <= halfWidth; i += stepW)
    {
        vertices.push_back({});
        
        for (float j = -halfHeight; j <= halfHeight; j += stepH)
        {
            vertices.back().push_back(table.addVertex({ i, j, cz }));
        }
    }
            
    for (int row = 0, row_next = 1; row_next < vertices.size(); ++row, ++row_next)
    {
        for (int col = 0, col_next = 1; col_next < vertices[row_next].size(); ++col, ++col_next)
        {
            table.addFace(vertices[row][col], vertices[row][col_next], vertices[row_next][col]);
            table.addFace(vertices[row_next][col], vertices[row][col_next], vertices[row_next][col_next]);
        }
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.m_vertices)
            vertex.data = mat * glm::vec4(vertex.data, 1.0f);
    }

    table.connectTwins();

    return table;
}
