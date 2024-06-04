#pragma once

#include "MeshEngine/RenderSystem/RenderSystem.h"

#include <unordered_map>

class GLRenderSystem : public RenderSystem
{
public:
    virtual ~GLRenderSystem();
    
    void init() override;

    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    virtual void setPolygonMode(bool fill) override;
    virtual void setPointSize(float size) override;
    virtual void setLineSize(float size) override;
    
    // Clear
    virtual void clearDepth(float d = 1.0f) override;
    virtual void clearColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f) override;
    virtual void clearDisplay(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f, float d = 1.0f) override;
    
    // Buffer
    virtual uint32_t bufferData(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) override;
    virtual void unbufferData(uint32_t bufferId) override;

    virtual void bufferSubData(uint32_t bufferId, uint32_t index, const Vertex& vertex) override;

    virtual uint32_t bufferTexture(const std::string& texturePath) override;
    virtual void unbufferTexture(uint32_t textureId) override;

    virtual void bufferFrame(uint32_t& bufferId, uint32_t& renderId, uint32_t& textureId, uint32_t width, uint32_t height) override;
    virtual void unbufferFrame(uint32_t bufferId) override;

    // Bind
    virtual void bindBuffer(uint32_t bufferId) override;
    virtual void unbindBuffer() override;

    virtual void bindTexture(uint32_t activeId, uint32_t textureId) override;
    virtual void unbindTexture() override;

    virtual void bindFrame(uint32_t bufferId) override;
    virtual void unbindFrame() override;

    // Render
    virtual void renderTriangles() override;
    virtual void renderQuads() override;
    virtual void renderPoints() override;
    virtual void renderLines() override;

    virtual void renderTriangles(uint32_t index, uint32_t count) override;
    virtual void renderQuads(uint32_t index, uint32_t count) override;
    virtual void renderPoints(uint32_t index, uint32_t count) override;
    virtual void renderLines(uint32_t index, uint32_t count) override;

private:
    // Store VAO, VBO, EBO
    std::unordered_map<uint32_t, std::tuple<unsigned int, unsigned int, unsigned int>> m_dataMap;

    // Store Frame, Render, Texture
    std::unordered_map<uint32_t, std::tuple<unsigned int, unsigned int, unsigned int>> m_frameMap;
};


