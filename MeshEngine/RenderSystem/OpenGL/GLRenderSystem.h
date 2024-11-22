#pragma once

#include <unordered_map>

#include "MeshEngine/RenderSystem/RenderSystem.h"

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
    virtual void unbufferData(uint32_t dataId) override;

    virtual void bufferSubData(uint32_t dataId, uint32_t index, const Vertex& vertex) override;

    virtual uint32_t bufferUniform(uint32_t bindPoint, uint32_t size, const void* data) override;
    virtual void unbufferUniform(uint32_t uniformId) override;

    virtual void bufferSubUniform(uint32_t uniformId, uint32_t offset, uint32_t size, const void* data) override;

    virtual uint32_t bufferTexture(const std::string& texturePath, bool flip = false) override;
    virtual uint32_t bufferTexture(const std::string& texturePath, uint32_t& width, uint32_t& height, bool flip = false) override;
    virtual void unbufferTexture(uint32_t textureId) override;

    virtual void bufferFrame(uint32_t& frameId, uint32_t& renderId, uint32_t& textureId, uint32_t width, uint32_t height) override;
    virtual void unbufferFrame(uint32_t frameId) override;

    virtual void bufferDepth(uint32_t& depthId, uint32_t& textureId, uint32_t width, uint32_t height) override;
    virtual void unbufferDepth(uint32_t depthId) override;

    // Bind
    virtual void bindData(uint32_t dataId) override;
    virtual void unbindData() override;

    virtual void bindTexture(uint32_t activeId, uint32_t textureId) override;
    virtual void unbindTexture() override;

    virtual void bindFrame(uint32_t frameId) override;
    virtual void unbindFrame() override;

    virtual void bindDepth(uint32_t depthId) override;
    virtual void unbindDepth() override;

    virtual void bindUniform(uint32_t uniformId) override;
    virtual void unbindUniform() override;

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

    // Store Path, Texture, Count
    std::unordered_map<std::string, std::pair<unsigned int, unsigned int>> m_textureMap;

    // Store Frame, Render, Texture
    std::unordered_map<uint32_t, std::tuple<unsigned int, unsigned int, unsigned int>> m_frameMap;

    // Store Depth, Texture
    std::unordered_map<uint32_t, std::tuple<unsigned int, unsigned int>> m_depthMap;
};


