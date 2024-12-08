#include "DrawHelper.h"

#include "MeshEngine/Misc/Settings.h"
#include "MeshEngine/Misc/Export.h"

#include "MeshEngine/RenderSystem/RenderSystem.h"
#include "MeshEngine/RenderSystem/Shader.h"

#include "MeshEngine/Base/Mesh.h"

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
    
    if (fill)
    {
        renderSystem->bufferSubData(renderTrianglesId, 0, { a, {}, {} });
        renderSystem->bufferSubData(renderTrianglesId, 1, { b, {}, {} });
        renderSystem->bufferSubData(renderTrianglesId, 2, { c, {}, {} });
    }
    else
    {
        renderSystem->bufferSubData(renderLinesId, 0, { a, {}, {} });
        renderSystem->bufferSubData(renderLinesId, 1, { b, {}, {} });
        renderSystem->bufferSubData(renderLinesId, 2, { c, {}, {} });
    }

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

    if (fill)
    {
        renderSystem->bufferSubData(renderTrianglesId, 0, { a, {}, {} });
        renderSystem->bufferSubData(renderTrianglesId, 1, { b, {}, {} });
        renderSystem->bufferSubData(renderTrianglesId, 2, { c, {}, {} });
        renderSystem->bufferSubData(renderTrianglesId, 3, { d, {}, {} });
    }
    else
    {
        renderSystem->bufferSubData(renderLinesId, 0, { a, {}, {} });
        renderSystem->bufferSubData(renderLinesId, 1, { b, {}, {} });
        renderSystem->bufferSubData(renderLinesId, 2, { c, {}, {} });
        renderSystem->bufferSubData(renderLinesId, 3, { d, {}, {} });
    }

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

    renderSystem->bufferSubData(renderId, 0, { start, {}, {} });
    renderSystem->bufferSubData(renderId, 1, { end, {}, {} });

    drawDebugInternal(renderSystem, renderId, glm::mat4(1.f), color, 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugSphere(RenderSystem* renderSystem, glm::vec3 position, float radius, glm::vec4 color, bool fill, float thickness)
{
    glm::mat4 translation = glm::translate(position);
    glm::mat4 scale = glm::scale(glm::vec3(radius));

    drawDebugSphereInternal(renderSystem, translation * scale, color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugSphere(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    glm::mat4 translation = glm::translate(position);
    glm::mat4 scale = glm::scale(size);

    drawDebugSphereInternal(renderSystem, translation * scale, color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, glm::vec3 position, glm::vec3 size, glm::vec4 color, bool fill, float thickness)
{
    glm::mat4 translation = glm::translate(position);
    glm::mat4 scale = glm::scale(size);

    drawDebugBoxInternal(renderSystem, translation * scale, color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugBox(RenderSystem* renderSystem, const glm::mat4& transform, glm::vec3 min, glm::vec3 max, glm::vec4 color, bool fill, float thickness)
{
    glm::mat4 translation = glm::translate((min + max) * 0.5f);
    glm::mat4 scale = glm::scale(glm::abs(max - min));
    
    drawDebugBoxInternal(renderSystem, transform * translation * scale, color, fill, thickness);
}

void MeshEngine::DrawHelper::drawDebugSphereInternal(RenderSystem* renderSystem, const glm::mat4& matrix, glm::vec4 color, bool fill, float thickness)
{
    static uint32_t renderTrianglesId = 0;
    static uint32_t renderLinesId = 0;

    static bool init = false;

    if (!init)
    {
        std::unique_ptr<Mesh> mesh = Mesh::createSphere(glm::vec3(0.f), 1.f, 16);

        renderTrianglesId = renderSystem->bufferData(mesh->getRenderVertices(), mesh->getRenderTriangles());
        renderLinesId = renderSystem->bufferData(mesh->getRenderVertices(), mesh->getRenderLines());

        init = true;
    }

    drawDebugInternal(renderSystem, fill ? renderTrianglesId : renderLinesId, matrix, color, fill ? 0 : 3, thickness);
}

void MeshEngine::DrawHelper::drawDebugBoxInternal(RenderSystem* renderSystem, const glm::mat4& matrix, glm::vec4 color, bool fill, float thickness)
{
    static uint32_t renderTrianglesId = 0;
    static uint32_t renderLinesId = 0;

    static bool init = false;

    if (!init)
    {
        std::unique_ptr<Mesh> mesh = Mesh::createCube(glm::vec3(0.f), 1.f);

        renderTrianglesId = renderSystem->bufferData(mesh->getRenderVertices(), mesh->getRenderTriangles());
        renderLinesId = renderSystem->bufferData(mesh->getRenderVertices(), mesh->getRenderLines());

        init = true;
    }

    drawDebugInternal(renderSystem, fill ? renderTrianglesId : renderLinesId, matrix, color, fill ? 0 : 3, thickness);
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
