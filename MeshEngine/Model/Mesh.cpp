#include "Mesh.h"

#include <set>
#include <numeric>

Mesh::Mesh(const heds::HalfEdgeTable<Vertex>& halfEdgeTable)
    : m_table(halfEdgeTable)
{
    update();
}

Mesh::Mesh(const heds::HalfEdgeTable<Vertex>& halfEdgeTable, const Material& material)
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
        
        rs.unbufferData(m_trianglesId);
        rs.unbufferData(m_linesId);
        rs.unbufferData(m_holesId);
        rs.unbufferData(m_boundariesId);
       
        rs.unbufferTexture(m_material.diffuseMap.id);
        rs.unbufferTexture(m_material.specularMap.id);
        rs.unbufferTexture(m_material.emissionMap.id);

        m_trianglesId = rs.bufferData(m_vertices, m_triangles);
        m_linesId = rs.bufferData(m_vertices, m_lines);
        m_holesId = rs.bufferData(m_vertices, m_holes);
        m_boundariesId = rs.bufferData(m_vertices, m_boundaries);

        m_material.diffuseMap.id = rs.bufferTexture(m_material.diffuseMap.path);
        m_material.specularMap.id = rs.bufferTexture(m_material.specularMap.path);
        m_material.emissionMap.id = rs.bufferTexture(m_material.emissionMap.path);
    }

    if (m_bufferSubData)
    {
        m_bufferSubData = false;

        for (auto& indice : m_subDataIndices)
        {
            rs.bufferSubData(m_trianglesId, indice, m_vertices[indice]);
            rs.bufferSubData(m_linesId, indice, m_vertices[indice]);
            rs.bufferSubData(m_holesId, indice, m_vertices[indice]);
            rs.bufferSubData(m_boundariesId, indice, m_vertices[indice]);
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

        shader.setInt("material.diffuseMap", 0);
        shader.setInt("material.specularMap", 1);
        shader.setInt("material.emissionMap", 2);
        
        rs.bindTexture(0, m_material.diffuseMap.id);
        rs.bindTexture(1, m_material.specularMap.id);
        rs.bindTexture(2, m_material.emissionMap.id);

        rs.bindData(m_trianglesId);
        rs.renderTriangles();
    }

    if (renderLines && !m_lines.empty())
    {
        shader.setVec3("material.ambient", glm::vec3(0.f));
        shader.setVec3("material.diffuse", glm::vec3(0.f));
        shader.setVec3("material.specular", glm::vec3(0.f));
        shader.setVec3("material.emission", colorLines);
        shader.setFloat("material.shininess", 0);

        rs.bindData(m_linesId);
        
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

        rs.bindData(m_holesId);

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

        rs.bindData(m_boundariesId);
        rs.renderTriangles();
    }

    rs.unbindTexture();
    rs.unbindData();   
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

    m_bbox.min = m_bbox.max = vertices.front().data.position;

    for (const auto& vertex : vertices)
    {
        const auto& data = vertex.data;

        m_bbox.min.x = std::min(m_bbox.min.x, data.position.x);
        m_bbox.min.y = std::min(m_bbox.min.y, data.position.y);
        m_bbox.min.z = std::min(m_bbox.min.z, data.position.z);

        m_bbox.max.x = std::max(m_bbox.max.x, data.position.x);
        m_bbox.max.y = std::max(m_bbox.max.y, data.position.y);
        m_bbox.max.z = std::max(m_bbox.max.z, data.position.z);

        m_vertices.push_back(data);
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
        center += m_table.getEndPoint(next_heh).position;
        next_heh = m_table.next(next_heh);

    } while (next_heh != start_heh);

    center /= static_cast<float>(vertices);

    do
    {
        heds::VertexHandle vh = m_table.destVertex(next_heh);

        Vertex data = m_table.getPoint(vh);
        data.position = glm::translate(-center) * glm::vec4(data.position, 1.0f);
        data.position = trf * glm::vec4(data.position, 1.0f);
        data.position = glm::translate(center) * glm::vec4(data.position, 1.0f);

        m_table.setPoint(vh, data);

        m_bbox.min.x = std::min(m_bbox.min.x, data.position.x);
        m_bbox.min.y = std::min(m_bbox.min.y, data.position.y);
        m_bbox.min.z = std::min(m_bbox.min.z, data.position.z);

        m_bbox.max.x = std::max(m_bbox.max.x, data.position.x);
        m_bbox.max.y = std::max(m_bbox.max.y, data.position.y);
        m_bbox.max.z = std::max(m_bbox.max.z, data.position.z);

        m_vertices[vh].position = data.position;

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
    
    Vertex center = m_table.getPoint(vh);

    Vertex data = m_table.getPoint(vh);
    data.position = glm::translate(-center.position) * glm::vec4(data.position, 1.0f);
    data.position = trf * glm::vec4(data.position, 1.0f);
    data.position = glm::translate(center.position) * glm::vec4(data.position, 1.0f);

    m_table.setPoint(vh, data);

    m_bbox.min.x = std::min(m_bbox.min.x, data.position.x);
    m_bbox.min.y = std::min(m_bbox.min.y, data.position.y);
    m_bbox.min.z = std::min(m_bbox.min.z, data.position.z);

    m_bbox.max.x = std::max(m_bbox.max.x, data.position.x);
    m_bbox.max.y = std::max(m_bbox.max.y, data.position.y);
    m_bbox.max.z = std::max(m_bbox.max.z, data.position.z);

    m_vertices[vh].position = data.position;
    
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

heds::HalfEdgeTable<Vertex>& Mesh::getHalfEdgeTable()
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

std::string& Mesh::getName()
{
    return m_name;
}

const Material& Mesh::getMaterial() const
{
    return m_material;
}

Material& Mesh::getMaterial()
{
    return m_material;
}

std::unique_ptr<Mesh> Mesh::createCube(glm::vec3 center, float length)
{
    heds::HalfEdgeTable<Vertex> table;
    float halfLength = length / 2;

    heds::VertexHandle vh0 = table.addVertex(Vertex{ glm::vec3(halfLength,  halfLength, -halfLength) + center, {}, {} });
    heds::VertexHandle vh1 = table.addVertex(Vertex{ glm::vec3(halfLength, -halfLength, -halfLength) + center, {}, {} });
    heds::VertexHandle vh2 = table.addVertex(Vertex{ glm::vec3(-halfLength, -halfLength, -halfLength) + center, {}, {} });
    heds::VertexHandle vh3 = table.addVertex(Vertex{ glm::vec3(-halfLength,  halfLength, -halfLength) + center, {}, {} });
    heds::VertexHandle vh4 = table.addVertex(Vertex{ glm::vec3(halfLength,  halfLength,  halfLength) + center, {}, {} });
    heds::VertexHandle vh5 = table.addVertex(Vertex{ glm::vec3(halfLength, -halfLength,  halfLength) + center, {}, {} });
    heds::VertexHandle vh6 = table.addVertex(Vertex{ glm::vec3(-halfLength, -halfLength,  halfLength) + center, {}, {} });
    heds::VertexHandle vh7 = table.addVertex(Vertex{ glm::vec3(-halfLength,  halfLength,  halfLength)  + center, {}, {} });

    table.addFace(vh0, vh1, vh2, vh3);
    table.addFace(vh4, vh7, vh6, vh5);
    table.addFace(vh0, vh4, vh5, vh1);
    table.addFace(vh1, vh5, vh6, vh2);
    table.addFace(vh2, vh6, vh7, vh3);
    table.addFace(vh4, vh0, vh3, vh7);

    table.connectTwins();

    return std::make_unique<Mesh>(table);
}

std::unique_ptr<Mesh> Mesh::createCylinder(glm::vec3 inDir, float R, float h, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    float cx = 0.0f, cy = 0.0f, cz = 0.0f, radius = R; int segments = numSubdivisions;

    std::vector<heds::VertexHandle> vertices;
    heds::HalfEdgeTable<Vertex> table;

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

    std::vector<heds::VertexHandle> vertices;
    heds::HalfEdgeTable<Vertex> table;

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

    std::vector<heds::VertexHandle> vertices;
    heds::HalfEdgeTable<Vertex> table;

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
    heds::HalfEdgeTable<Vertex> table1 = Mesh::createCone(inDir, R1, h1, numSubdivisions)->getHalfEdgeTable();
    heds::HalfEdgeTable<Vertex> table2 = Mesh::createCylinder(inDir, R2, h2, numSubdivisions)->getHalfEdgeTable();

    glm::vec3 delta = inDir * h2;

    for (auto& face : table1.getFaces())
    {
        heds::HalfEdgeHandle heh0 = face.heh;
        heds::HalfEdgeHandle heh1 = table1.next(heh0);
        heds::HalfEdgeHandle heh2 = table1.next(heh1);

        glm::vec3 vec0{ table1.getEndPoint(heh0).position }; vec0 += delta;
        glm::vec3 vec1{ table1.getEndPoint(heh1).position }; vec1 += delta;
        glm::vec3 vec2{ table1.getEndPoint(heh2).position }; vec2 += delta;

        heds::VertexHandle v0 = table2.addVertex({ {vec0}, {}, {} });
        heds::VertexHandle v1 = table2.addVertex({ {vec1}, {}, {} });
        heds::VertexHandle v2 = table2.addVertex({ {vec2}, {}, {} });

        table2.addFace(v0, v1, v2);
    }

    table2.connectTwins();

    return std::make_unique<Mesh>(table2);
}

std::unique_ptr<Mesh> Mesh::createPlane(glm::vec3 inDir, float width, float heigth, uint32_t numSubdivisions)
{
    const glm::vec3 dir(0, 0, 1);
    int segments = glm::sqrt(numSubdivisions);
    float stepW = width / segments;
    float stepH = heigth / segments;
    float halfWidth = width / 2;
    float halfHeight = heigth / 2;
    float cx = 0.0f, cy = 0.0f, cz = 0.0f;

    std::vector<std::vector<heds::VertexHandle>> vertices;
    heds::HalfEdgeTable<Vertex> table;

    for (float i = -halfWidth; i <= halfWidth; i += stepW)
    {
        vertices.push_back({});

        for (float j = -halfHeight; j <= halfHeight; j += stepH)
        {
            vertices.back().push_back(table.addVertex({ { i, j, cz }, {}, {} }));
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

        for (auto& vertex : table.getVertices())
            vertex.data.position = mat * glm::vec4(vertex.data.position, 1.0f);
    }

    table.connectTwins();

    return std::make_unique<Mesh>(table);;
}