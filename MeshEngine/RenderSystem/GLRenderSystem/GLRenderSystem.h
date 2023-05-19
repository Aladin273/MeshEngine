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
    void setViewport(double x, double y, double width, double height) override;

    void renderTriangles(const std::vector<Vertex>& vertices) override;
    void renderTriangles(const std::vector<Vertex>& vertices, glm::vec3 color) override;
    void renderLines(const std::vector<Vertex>& vertices) override;
    void renderLines(const std::vector<Vertex>& vertices, glm::vec3 color) override;

    void setupMaterial(const Material& material, Face face = Face::Front) override;
    void setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is) override;
    void setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float k_const = 1.0f, float k_linear = 0.0f, float k_quadratic = 0.0f) override;
    void turnLight(uint32_t index, bool enable) override;

    void setDepth(bool enable) override;
    void setSmooth(bool enable) override;
    void setLighting(bool enable) override;
    void setLightTwoSide(bool enable) override;
    void setColorMaterial(bool enable) override;

    bool getDepth() override;
    bool getSmooth() override;
    bool getLighting() override;
    bool getLightTwoSide() override;
    bool getColorMaterial() override;
    bool turnedLight(uint32_t index) override;

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

    const uint32_t m_maxIndex = 7;
    const float m_lineWidth = 2.0f;
};


