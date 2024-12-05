#include "Mesh.h"

Mesh::Mesh(const HalfEdgeTable<Vertex>& halfEdgeTable)
    : m_table(halfEdgeTable)
{
    m_name = "Mesh";
    updateData();
}

Mesh::Mesh(const HalfEdgeTable<Vertex>& halfEdgeTable, const Material& material)
    : m_table(halfEdgeTable), m_material(material)
{
    m_name = "Mesh";
    updateData();
}

void Mesh::updateData()
{
    m_renderDataDirty = true;
    m_renderSubDataDirty = true;

    m_renderVertices.clear();
    m_renderTriangles.clear();
    m_renderLines.clear();
    m_renderHoles.clear();
    m_renderBoundaries.clear();
    m_renderSubData.clear();

    if (m_table.getVertices().empty()) return;

    const auto& vertices = m_table.getVertices();
    const auto& faces = m_table.getFaces();

    m_renderVertices.reserve(vertices.size());
    m_renderTriangles.reserve(faces.size() * 6);
    m_renderLines.reserve(faces.size() * 8);
    m_renderHoles.reserve(faces.size() * 8);
    m_renderBoundaries.reserve(faces.size() * 6);

    for (const auto& vertex : vertices)
    {
        m_renderVertices.push_back(vertex.data);
    }

    std::vector<std::pair<glm::vec3, float>> normalsMap{ 0 };
    normalsMap.resize(vertices.size());

    for (const auto& face : faces)
    {
        HalfEdgeVertexHandle vhs[4];
        HalfEdgeHandle hehs[4];

        hehs[0] = face.heh;
        for (int i = 1; i < 4; ++i)
            hehs[i] = m_table.next(hehs[i - 1]);

        for (int i = 0; i < 4; ++i)
            vhs[i] = m_table.sourceVertex(hehs[i]);

        glm::vec3 ab = m_renderVertices[vhs[1]].position - m_renderVertices[vhs[0]].position;
        glm::vec3 bc = m_renderVertices[vhs[2]].position - m_renderVertices[vhs[1]].position;
        glm::vec3 normal = glm::cross(ab, bc);

        for (const auto& vh : vhs)
        {
            normalsMap[vh].first += normal;
            normalsMap[vh].second += 1;
        }

        m_renderTriangles.push_back(vhs[0]);
        m_renderTriangles.push_back(vhs[1]);
        m_renderTriangles.push_back(vhs[2]);

        m_renderLines.push_back(vhs[0]);
        m_renderLines.push_back(vhs[1]);
        m_renderLines.push_back(vhs[1]);
        m_renderLines.push_back(vhs[2]);
        m_renderLines.push_back(vhs[2]);
        m_renderLines.push_back(vhs[3]);

        if (hehs[3] != hehs[0])
        {
            m_renderTriangles.push_back(vhs[2]);
            m_renderTriangles.push_back(vhs[3]);
            m_renderTriangles.push_back(vhs[0]);

            m_renderLines.push_back(vhs[3]);
            m_renderLines.push_back(vhs[0]);
        }

        bool boundary = false;

        for (size_t i = 0; i < 4; ++i)
        {
            if (m_table.deref(m_table.twin(hehs[i])).fh.index == invalid)
            {
                boundary = true;

                m_renderHoles.push_back(vhs[i]);
                m_renderHoles.push_back(vhs[i == 3 ? 0 : i + 1]);
            }
        }

        if (boundary)
        {
            m_renderBoundaries.push_back(vhs[0]);
            m_renderBoundaries.push_back(vhs[1]);
            m_renderBoundaries.push_back(vhs[2]);
            m_renderBoundaries.push_back(vhs[2]);
            m_renderBoundaries.push_back(vhs[3]);
            m_renderBoundaries.push_back(vhs[0]);
        }
    }

    for (size_t i = 0; i < normalsMap.size(); ++i)
        m_renderVertices[i].normal = glm::normalize(normalsMap[i].first / normalsMap[i].second);

    updateBbox();
    super::updateData();
}

void Mesh::updateSubData()
{
    for (auto& fh : m_affectedFaces)
    {
        if (fh != invalid)
        {
            HalfEdgeHandle start_heh = m_table.deref(fh).heh;
            HalfEdgeHandle next_heh = start_heh;

            do
            {
                m_affectedVertices.insert(m_table.destVertex(next_heh));
                next_heh = m_table.next(next_heh);

            } while (next_heh != start_heh);
        }
    }

    for (auto& vh : m_affectedVertices)
    {
        if (vh != invalid)
        {
            std::set<HalfEdgeFaceHandle> adjacentFaces;

            HalfEdgeHandle start_heh = m_table.deref(vh).heh;
            HalfEdgeHandle next_heh = start_heh;

            do
            {
                adjacentFaces.insert(m_table.deref(next_heh).fh);
                next_heh = m_table.next(m_table.twin(next_heh));

            } while (next_heh != start_heh);

            glm::vec3 normalsSum(0.f);

            for (auto& fh : adjacentFaces)
            {
                if (fh != invalid)
                {
                    HalfEdgeHandle heh0 = m_table.deref(fh).heh;
                    HalfEdgeHandle heh1 = m_table.next(heh0);
                    HalfEdgeHandle heh2 = m_table.next(heh1);

                    glm::vec3 ab = m_renderVertices[m_table.sourceVertex(heh1)].position - m_renderVertices[m_table.sourceVertex(heh0)].position;
                    glm::vec3 bc = m_renderVertices[m_table.sourceVertex(heh2)].position - m_renderVertices[m_table.sourceVertex(heh1)].position;
                    normalsSum += glm::cross(ab, bc);
                }
            }

            m_renderVertices[vh].normal = glm::normalize(normalsSum / static_cast<float>(adjacentFaces.size()));
            m_renderSubData.push_back(vh);
        }
    }

    updateBbox();
    super::updateSubData();
}

void Mesh::updateBbox()
{
    if (m_table.getVertices().size())
    {
        m_bbox.setMin(m_table.getVertices().front().data.position);
        m_bbox.setMax(m_table.getVertices().front().data.position);

        for (const auto& vertex : m_table.getVertices())
        {
            m_bbox.merge(vertex.data.position);
        }
    }
}

void Mesh::applyTransformation(HalfEdgeFaceHandle fh, const glm::mat4& trf)
{
    m_renderSubDataDirty = true;
    m_renderSubData.clear();

    glm::vec3 center{ 0.f }; 
    uint32_t vertices = 0;
    
    HalfEdgeHandle start_heh = m_table.deref(fh).heh;
    HalfEdgeHandle next_heh = start_heh;

    do
    {
        ++vertices;
        center += m_table.getEndPoint(next_heh).position;
        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);

    center /= static_cast<float>(vertices);

    do
    {
        HalfEdgeVertexHandle vh = m_table.destVertex(next_heh);

        Vertex data = m_table.getPoint(vh);;
        data.position = glm::vec3(trf * glm::vec4(data.position - center, 1.f)) + center;

        m_table.setPoint(vh, data);

        m_renderVertices[vh].position = data.position;

        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);


    m_affectedFaces.clear();
    m_affectedVertices.clear();

    do
    {
        HalfEdgeVertexHandle vh = m_table.destVertex(next_heh);

        HalfEdgeHandle inner_start_heh = m_table.deref(vh).heh;
        HalfEdgeHandle inner_next_heh = inner_start_heh;

        do
        {
            m_affectedFaces.insert(m_table.deref(inner_next_heh).fh);
            inner_next_heh = m_table.next(m_table.twin(inner_next_heh));

        } while (inner_next_heh != inner_start_heh);

        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);

    updateSubData();
}

void Mesh::applyTransformation(HalfEdgeVertexHandle vh, const glm::mat4& trf)
{
    m_renderSubDataDirty = true;
    m_renderSubData.clear();

    Vertex data = m_table.getPoint(vh);
    data.position = trf * glm::vec4(data.position, 1.0f);

    m_table.setPoint(vh, data);

    m_renderVertices[vh].position = data.position;

    m_affectedFaces.clear();
    m_affectedVertices.clear();

    HalfEdgeHandle start_heh = m_table.deref(vh).heh;
    HalfEdgeHandle next_heh = start_heh;

    do
    {
        m_affectedFaces.insert(m_table.deref(next_heh).fh);
        next_heh = m_table.next(m_table.twin(next_heh));

    } while (next_heh != start_heh);

    updateSubData();
}

void Mesh::deleteFace(HalfEdgeFaceHandle fh)
{
    m_table.deleteFace(fh);
    updateData();
}

HalfEdgeTable<Vertex>& Mesh::getHalfEdgeTable()
{
    return m_table;
}

const BoundingBox& Mesh::getBoundingBox() const
{
    return m_bbox;
}

void Mesh::setMaterial(const Material& material)
{
    m_material = material;
}

const Material& Mesh::getMaterial() const
{
    return m_material;
}

std::unique_ptr<Mesh> Mesh::createCube(glm::vec3 center, float length)
{
    HalfEdgeTable<Vertex> table;
    float halfLength = length / 2;

    HalfEdgeVertexHandle vh0 = table.addVertex(Vertex{ glm::vec3(halfLength,  halfLength, -halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh1 = table.addVertex(Vertex{ glm::vec3(halfLength, -halfLength, -halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh2 = table.addVertex(Vertex{ glm::vec3(-halfLength, -halfLength, -halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh3 = table.addVertex(Vertex{ glm::vec3(-halfLength,  halfLength, -halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh4 = table.addVertex(Vertex{ glm::vec3(halfLength,  halfLength,  halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh5 = table.addVertex(Vertex{ glm::vec3(halfLength, -halfLength,  halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh6 = table.addVertex(Vertex{ glm::vec3(-halfLength, -halfLength,  halfLength) + center, {}, {} });
    HalfEdgeVertexHandle vh7 = table.addVertex(Vertex{ glm::vec3(-halfLength,  halfLength,  halfLength)  + center, {}, {} });

    table.addFace(vh0, vh1, vh2, vh3);
    table.addFace(vh4, vh7, vh6, vh5);
    table.addFace(vh0, vh4, vh5, vh1);
    table.addFace(vh1, vh5, vh6, vh2);
    table.addFace(vh2, vh6, vh7, vh3);
    table.addFace(vh4, vh0, vh3, vh7);

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createSphere(glm::vec3 center, float radius, uint32_t numSubdivisions)
{
    std::vector<HalfEdgeVertexHandle> vertexHandles;
    HalfEdgeTable<Vertex> table;

    for (int lat = 0; lat <= numSubdivisions; ++lat)
    {
        float theta = glm::pi<float>() * lat / numSubdivisions;
        float sinTheta = glm::sin(theta);
        float cosTheta = glm::cos(theta);

        for (int lon = 0; lon <= numSubdivisions; ++lon)
        {
            float phi = glm::two_pi<float>() * lon / numSubdivisions;
            float sinPhi = glm::sin(phi);
            float cosPhi = glm::cos(phi);

            glm::vec3 position = glm::vec3(radius * sinTheta * cosPhi, radius * cosTheta, radius * sinTheta * sinPhi) + center;

            vertexHandles.push_back(table.addVertex(Vertex{ position, {}, {} }));
        }
    }

    for (int lat = 0; lat < numSubdivisions; ++lat)
    {
        for (int lon = 0; lon < numSubdivisions; ++lon)
        {
            int current = lat * (numSubdivisions + 1) + lon;
            int next = current + numSubdivisions + 1;

            HalfEdgeVertexHandle v0 = vertexHandles[current];
            HalfEdgeVertexHandle v1 = vertexHandles[current + 1];
            HalfEdgeVertexHandle v2 = vertexHandles[next + 1];
            HalfEdgeVertexHandle v3 = vertexHandles[next];

            if (lat != 0)
            {
                table.addFace(v0, v1, v2);
            }

            if (lat != numSubdivisions - 1)
            {
                table.addFace(v0, v2, v3);
            }
        }
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createCylinder(glm::vec3 inDir, float R, float h, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f, cz = 0.0f, radius = R; int segments = numSubdivisions;

    std::vector<HalfEdgeVertexHandle> vertices;
    HalfEdgeTable<Vertex> table;

    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(segments);

        float x = radius * glm::cos(theta);
        float y = radius * glm::sin(theta);

        vertices.push_back(table.addVertex({ {x,y, h }, {}, {} }));
        vertices.push_back(table.addVertex({ {x,y, cz}, {}, {} }));
    }

    vertices.push_back(vertices[0]);
    vertices.push_back(vertices[1]);
    vertices.push_back(table.addVertex({ { cx, cy, h }, {}, {} }));
    vertices.push_back(table.addVertex({ { cx, cy, cz}, {}, {} }));

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

        for (auto& vertex : table.getVertices())
            vertex.data.position = mat * glm::vec4(vertex.data.position, 1.0f);
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createCone(glm::vec3 inDir, float R, float h, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f, cz = 0.0f, radius = R; int segments = numSubdivisions;

    std::vector<HalfEdgeVertexHandle> vertices;
    HalfEdgeTable<Vertex> table;

    for (int i = 0; i < segments; ++i)
    {
        float theta = 2.0f * glm::pi<float>() * float(i) / float(segments);

        float x = radius * glm::cos(theta);
        float y = radius * glm::sin(theta);

        vertices.push_back(table.addVertex({ { x, y, cz }, {}, {} }));
    }

    vertices.push_back(vertices[0]);
    vertices.push_back(table.addVertex({ { cx, cy, h }, {}, {} }));
    vertices.push_back(table.addVertex({ { cx, cy, cz}, {}, {} }));

    for (size_t i = 0; i + 1 < vertices.size(); ++i)
    {
        table.addFace(vertices[vertices.size() - 2], vertices[i], vertices[i + 1]);
        table.addFace(vertices[vertices.size() - 1], vertices[i + 1], vertices[i]);
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.getVertices())
            vertex.data.position = mat * glm::vec4(vertex.data.position, 1.0f);
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createTorus(glm::vec3 inDir, float minorRadius, float majorRadius, uint32_t majorSegments)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f; int segments = static_cast<int>(majorSegments);

    std::vector<HalfEdgeVertexHandle> vertices;
    HalfEdgeTable<Vertex> table;

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

            vertices.push_back(table.addVertex({ { surfacePosition }, {}, {} }));

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

        for (auto& vertex : table.getVertices())
            vertex.data.position = mat * glm::vec4(vertex.data.position, 1.0f);
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createArrow(glm::vec3 inDir, float R1, float h1, float R2, float h2, uint32_t numSubdivisions)
{
    HalfEdgeTable<Vertex> table1 = Mesh::createCone(inDir, R1, h1, numSubdivisions)->getHalfEdgeTable();
    HalfEdgeTable<Vertex> table2 = Mesh::createCylinder(inDir, R2, h2, numSubdivisions)->getHalfEdgeTable();

    glm::vec3 delta = inDir * h2;

    for (auto& face : table1.getFaces())
    {
        HalfEdgeHandle heh0 = face.heh;
        HalfEdgeHandle heh1 = table1.next(heh0);
        HalfEdgeHandle heh2 = table1.next(heh1);

        glm::vec3 vec0{ table1.getEndPoint(heh0).position }; vec0 += delta;
        glm::vec3 vec1{ table1.getEndPoint(heh1).position }; vec1 += delta;
        glm::vec3 vec2{ table1.getEndPoint(heh2).position }; vec2 += delta;

        HalfEdgeVertexHandle v0 = table2.addVertex({ {vec0}, {}, {} });
        HalfEdgeVertexHandle v1 = table2.addVertex({ {vec1}, {}, {} });
        HalfEdgeVertexHandle v2 = table2.addVertex({ {vec2}, {}, {} });

        table2.addFace(v0, v1, v2);
    }

    table2.connectTwins();

    return std::make_unique<Mesh>(table2);
}

std::unique_ptr<Mesh> Mesh::createPlane(glm::vec3 inDir, float width, float height, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    int segments = glm::sqrt(numSubdivisions);
    float stepW = width / segments;
    float stepH = height / segments;
    float halfWidth = width / 2;
    float halfHeight = height / 2;
    float cz = 0.0f;

    std::vector<std::vector<HalfEdgeVertexHandle>> vertices;
    HalfEdgeTable<Vertex> table;

    for (float i = -halfWidth; i <= halfWidth; i += stepW)
    {
        vertices.push_back({});

        for (float j = -halfHeight; j <= halfHeight; j += stepH)
        {
            vertices.back().push_back(table.addVertex({ { i, j, cz }, {}, {} }));
        }
    }

    for (int row = 0; row < vertices.size() - 1; ++row)
    {
        for (int col = 0; col < vertices[row].size() - 1; ++col)
        {
            table.addFace(vertices[row][col], vertices[row][col + 1], vertices[row + 1][col + 1], vertices[row + 1][col]);
        }
    }

    if (dir != inDir)
    {
        const glm::mat4 mat = glm::rotate(glm::acos(glm::dot(dir, inDir)), glm::cross(dir, inDir));

        for (auto& vertex : table.getVertices())
        {
            vertex.data.position = mat * glm::vec4(vertex.data.position, 1.0f);
        }
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}
