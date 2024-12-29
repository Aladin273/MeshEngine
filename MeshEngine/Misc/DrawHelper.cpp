#include "DrawHelper.h"

#include <vector>
#include <unordered_map>

#include "MeshEngine/Misc/Settings.h"
#include "MeshEngine/Misc/Export.h"

#include "MeshEngine/RenderSystem/RenderSystem.h"
#include "MeshEngine/RenderSystem/Shader.h"

#include "MeshEngine/Base/Mesh.h"

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

void MeshEngine::DrawHelper::drawDebugTriangle(RenderSystem* renderSystem, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec4 color, bool fill, float thickness)
{
    static uint32_t renderTrianglesId = 0;
    static uint32_t renderLinesId = 0;

    static bool init = false;

    if (!init)
    {
        renderTrianglesId = renderSystem->bufferData({ {}, {}, {} }, { 0, 1, 2 });
        renderLinesId = renderSystem->bufferData({ {}, {}, {} }, { 0, 1, 1, 2, 2, 0 });
        
        init = true;
    }
    
    renderSystem->bufferSubData(fill ? renderTrianglesId : renderLinesId, 0, { { a, {}, {} }, { b, {}, {} }, { c, {}, {} } });

    drawDebugInternal(renderSystem, fill ? renderTrianglesId : renderLinesId, glm::mat4(1.f), color, fill ? 0 : 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugQuad(RenderSystem* renderSystem, glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d, glm::vec4 color, bool fill, float thickness)
{
    static uint32_t renderTrianglesId = 0;
    static uint32_t renderLinesId = 0;

    static bool init = false;

    if (!init)
    {
        renderTrianglesId = renderSystem->bufferData({ {}, {}, {}, {} }, { 0, 1, 2, 2, 3, 0 });
        renderLinesId = renderSystem->bufferData({ {}, {}, {}, {}, }, { 0, 1, 1, 2, 2, 3, 3, 0 });

        init = true;
    }

    renderSystem->bufferSubData(fill ? renderTrianglesId : renderLinesId, 0, { { a, {}, {} }, { b, {}, {} }, { c, {}, {} }, { d, {}, {} } });

    drawDebugInternal(renderSystem, fill ? renderTrianglesId : renderLinesId, glm::mat4(1.f), color, fill ? 0 : 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugPoint(RenderSystem* renderSystem, glm::vec3 point, glm::vec4 color, float thickness)
{
    static uint32_t renderId = 0;
    static bool init = false;

    if (!init)
    {
        renderId = renderSystem->bufferData({ {} }, { 0 });
        init = true;
    }

    renderSystem->bufferSubData(renderId, 0, { point, {}, {} });

    drawDebugInternal(renderSystem, renderId, glm::mat4(1.f), color, 2, thickness);
}

void MeshEngine::DrawHelper::drawDebugLine(RenderSystem* renderSystem, glm::vec3 start, glm::vec3 end, glm::vec4 color, float thickness)
{
    static uint32_t renderId = 0;
    static bool init = false;

    if (!init)
    {
        renderId = renderSystem->bufferData({ {}, {} }, { 0, 1 });
        init = true;
    }

    renderSystem->bufferSubData(renderId, 0, { { start, {}, {} }, { end, {}, {} } });

    drawDebugInternal(renderSystem, renderId, glm::mat4(1.f), color, 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 0, glm::translate(position) * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 min, glm::vec3 max, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 0, glm::translate(position) * glm::scale(glm::abs(max - min)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 0, transform * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 min, glm::vec3 max, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 0, transform * glm::translate((min + max) * 0.5f) * glm::scale(glm::abs(max - min)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugSphere(RenderSystem* renderSystem, glm::vec3 position, float radius, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 1, glm::translate(position) * glm::scale(glm::vec3(radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugSphere(RenderSystem* renderSystem, const glm::mat4& transform, float radius, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 1, transform * glm::scale(glm::vec3(radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugEllipsoid(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 2, glm::translate(position) * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugEllipsoid(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 2, transform * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugCone(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 dir, float radius, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 3, glm::translate(position) * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius, height, radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugCone(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 dir, float radius, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 3, transform * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius, height, radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugCylinder(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 dir, float radius, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 4, glm::translate(position) * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius, height, radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugCylinder(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 dir, float radius, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 4, transform * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius, height, radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugTorus(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 dir, float radius, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 5, glm::translate(position) * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugTorus(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 dir, float radius, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 5, transform * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(radius)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugArrow(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 dir, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 6, glm::translate(position) * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugArrow(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 dir, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 6, transform * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(size), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugPlane(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 dir, float width, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 7, glm::translate(position) * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(width, 1.f, height)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugPlane(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 dir, float width, float height, glm::vec4 color, bool fill, float thickness)
{
    drawDebugShapeInternal(renderSystem, 7, transform * glm::mat4_cast(glm::rotation(glm::normalize(glm::vec3(0.f, 1.f, 0.f)), glm::normalize(dir))) * glm::scale(glm::vec3(width, 1.f, height)), color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugShapeInternal(RenderSystem* renderSystem, uint32_t shape, const glm::mat4& matrix, glm::vec4 color, bool fill, float thickness)
{
    static std::unordered_map<uint32_t, std::pair<uint32_t, uint32_t>> meshMap;
    static bool init = false;

    if (!init)
    {
        std::vector<std::unique_ptr<Mesh>> meshArray;

        meshArray.push_back(std::move(Mesh::createBox(glm::vec3(0.f), 1.f, 1.f, 1.f)));
        meshArray.push_back(std::move(Mesh::createSphere(glm::vec3(0.f), 1.f, 16)));
        meshArray.push_back(std::move(Mesh::createEllipsoid(glm::vec3(0.f), glm::vec3(1.f), 16)));
        meshArray.push_back(std::move(Mesh::createCone(glm::vec3(0.f, 1.f, 0.f), 1.f, 1.f, 16)));
        meshArray.push_back(std::move(Mesh::createCylinder(glm::vec3(0.f, 1.f, 0.f), 1.f, 1.f, 16)));
        meshArray.push_back(std::move(Mesh::createTorus(glm::vec3(0.f, 1.f, 0.f), 0.25f, 1.f, 16)));
        meshArray.push_back(std::move(Mesh::createArrow(glm::vec3(0.f, 1.f, 0.f), 0.125f, 0.25f, 0.0625f, 0.75f, 16)));
        meshArray.push_back(std::move(Mesh::createPlane(glm::vec3(0.f, 1.f, 0.f), 1.f, 1.f, 16)));

        for (uint8_t i = 0; i < meshArray.size(); ++i)
        {
            meshMap[i].first = renderSystem->bufferData(meshArray[i]->getRenderVertices(), meshArray[i]->getRenderTriangles());
            meshMap[i].second = renderSystem->bufferData(meshArray[i]->getRenderVertices(), meshArray[i]->getRenderLines());
        }

        init = true;
    }

    drawDebugInternal(renderSystem, fill ? meshMap[shape].first : meshMap[shape].second, matrix, color, fill ? 0 : 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugInternal(RenderSystem* renderSystem, uint32_t renderId, const glm::mat4& matrix, glm::vec4 color, uint32_t type, float thickness)
{
    static Shader* shader = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseColorVertex.glsl", MeshEngine::Settings::shadersPath + "baseColorFragment.glsl");

    shader->bind();

    shader->setMat4("model", matrix);
    shader->setVec4("color", color);

    renderSystem->bindData(renderId);

    if (type == 0)
    {
        renderSystem->renderTriangles();
    }
    else if (type == 1)
    {
        renderSystem->renderQuads();
    }
    else if (type == 2)
    {
        renderSystem->setPointSize(thickness);
        renderSystem->renderPoints();
    }
    else if (type == 3)
    {
        renderSystem->setLineSize(thickness);
        renderSystem->renderLines();
    }

    shader->unbind();
    renderSystem->unbindData();
}
