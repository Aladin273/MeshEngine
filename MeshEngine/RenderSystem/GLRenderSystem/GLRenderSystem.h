#pragma once

#include "ThirdParty/glad/glad.h"
#include "ThirdParty/glm/gtc/type_ptr.hpp"

#include "RenderSystem/IRenderSystem.h"

class GLRenderSystem : public IRenderSystem
{
public:
    virtual ~GLRenderSystem();
    void init() override;

    void clearDepth(float d = 1.0f);
    void clearColor(float r, float g, float b, float a = 1.0f);
    void clearDisplay(float r, float g, float b, float a = 1.0f, float d = 1.0f) override;
    void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

    void renderTriangles(const std::vector<Vertex>& vertices) override;
    void renderTriangles(const std::vector<Vertex>& vertices, glm::vec4 color) override;
    void renderPolygons(const std::vector<Vertex>& vertices) override;
    void renderPolygons(const std::vector<Vertex>& vertices, glm::vec4 color) override;
    void renderQuads(const std::vector<Vertex>& vertices) override;
    void renderQuads(const std::vector<Vertex>& vertices, glm::vec4 color) override;
    
    void renderPoints(const std::vector<Vertex>& vertices, float size = -1.0f) override;
    void renderPoints(const std::vector<Vertex>& vertices, glm::vec4 color, float size = -1.0f) override;
    void renderLines(const std::vector<Vertex>& vertices, float size = -1.0f) override;
    void renderLines(const std::vector<Vertex>& vertices, glm::vec4 color, float size = -1.0f) override;

    void setupMaterial(const Material& material, Face face = Face::Front) override;
    void setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is) override;
    void setupLightSpot(uint32_t index, glm::vec3 position, glm::vec3 direction, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float cutoff = 15.0f, float exponent = 20.0f) override;
    void setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float constant = 1.0f, float linear = 0.0f, float quadratic = 0.0f) override;

    void setLight(uint32_t index, bool enable) override;
    void setLighting(bool enable) override;
    void setColorMaterial(bool enable) override;
    void setDepth(bool enable) override;
    void setSmooth(bool enable) override;

    bool getLight(uint32_t index) const override;
    bool getLighting() const override;
    bool getColorMaterial() const override;
    bool getDepth() const override;
    bool getSmooth() const override;

    void setPointSize(float size) override;
    void setLineSize(float size) override;

    float getPointSize() const override;
    float getLineSize() const override;

    void setWorldMatrix(const glm::mat4& matrix) override;
    void setViewMatrix(const glm::mat4& matrix) override;
    void setProjMatrix(const glm::mat4& matrix) override;

    const glm::mat4& getWorldMatrix() const override;
    const glm::mat4& getViewMatrix() const override;
    const glm::mat4& getProjMatrix() const override;

private:
    glm::mat4 m_wordl_matrix{ 1.0f };
    glm::mat4 m_view_matrix{ 1.0f };
    glm::mat4 m_proj_matrix{ 1.0f };

    float m_lineSize = 2.0f;
    float m_pointSize = 2.0f;
};


