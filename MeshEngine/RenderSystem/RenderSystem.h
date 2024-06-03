#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position{ 0.f };
    glm::vec3 normal{ 0.f };
    glm::vec2 texCoords {0.f};
};

class RenderSystem
{
public:
    virtual ~RenderSystem() {}
    
    virtual void init() = 0;

    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void setPolygonMode(bool fill) = 0;
    virtual void setPointSize(float size) = 0;
    virtual void setLineSize(float size) = 0;

    // Clear
    virtual void clearDepth(float d = 1.0f) = 0;
    virtual void clearColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) = 0;
    virtual void clearDisplay(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, float d = 1.0f) = 0;

    // Buffer
    virtual uint32_t bufferData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) = 0;
    virtual void unbufferData(uint32_t bufferId) = 0;

    virtual void bufferSubData(uint32_t bufferId, uint32_t index, const Vertex& vertex) = 0;

    virtual uint32_t bufferTexture(const std::string& texturePath) = 0;
    virtual void unbufferTexture(uint32_t textureId) = 0;

    virtual void bufferFrame(uint32_t& bufferId, uint32_t& renderId, uint32_t& textureId, uint32_t width, uint32_t height) = 0;
    virtual void unbufferFrame(uint32_t bufferId) = 0;

    // Bind
    virtual void bindBuffer(uint32_t bufferId) = 0;
    virtual void unbindBuffer() = 0;

    virtual void bindTexture(uint32_t activeId, uint32_t textureId) = 0;
    virtual void unbindTexture() = 0;

    virtual void bindFrame(uint32_t bufferId) = 0;
    virtual void unbindFrame() = 0;

    // Render
    virtual void renderTriangles() = 0;
    virtual void renderQuads() = 0;
    virtual void renderPoints() = 0;
    virtual void renderLines() = 0;

    virtual void renderTriangles(uint32_t index, uint32_t count) = 0;
    virtual void renderQuads(uint32_t index, uint32_t count) = 0;
    virtual void renderPoints(uint32_t index, uint32_t count) = 0;
    virtual void renderLines(uint32_t index, uint32_t count) = 0;
};