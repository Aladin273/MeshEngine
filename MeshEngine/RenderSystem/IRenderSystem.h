#pragma once

#include <vector>
#include "ThirdParty/glm/glm.hpp"

struct Vertex
{
    glm::vec3 position{ 0.0f };
    glm::vec3 normal{ 0.0f };
    glm::vec4 color{ 1.0f };
};

struct Material
{
    glm::vec3 ambient{ 0.0f };
    glm::vec4 diffuse{ 1.0f };
    glm::vec3 specular{ 1.0f };
    glm::vec3 emission{ 0.0f };
    float shininess{ 128.0f };
};

enum class Face
{
    Front = 1028,
    Back = 1029,
    Front_and_Back = 1032
};

enum class Shade
{
    Smooth = 0,
    Flat = 1,
};

class IRenderSystem
{
public:
    virtual ~IRenderSystem() {}
    virtual void init() = 0;

    virtual void clearDepth(float d = 1.0f) = 0;
    virtual void clearColor(float r, float g, float b, float a = 1.0f) = 0;
    virtual void clearDisplay(float r, float g, float b, float a = 1.0f, float d = 1.0f) = 0;
    virtual void setViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    virtual void renderTriangles(const std::vector<Vertex>& vertices) = 0;
    virtual void renderTriangles(const std::vector<Vertex>& vertices, glm::vec4 color) = 0;
    virtual void renderQuads(const std::vector<Vertex>& vertices) = 0;
    virtual void renderQuads(const std::vector<Vertex>& vertices, glm::vec4 color) = 0;
    virtual void renderPolygons(const std::vector<Vertex>& vertices) = 0;
    virtual void renderPolygons(const std::vector<Vertex>& vertices, glm::vec4 color) = 0;
    
    virtual void renderPoints(const std::vector<Vertex>& vertices, float size = -1.0f) = 0;
    virtual void renderPoints(const std::vector<Vertex>& vertices, glm::vec4 color, float size = -1.0f) = 0;
    virtual void renderLines(const std::vector<Vertex>& vertices, float size = -1.0f) = 0;
    virtual void renderLines(const std::vector<Vertex>& vertices, glm::vec4 color, float size = -1.0f) = 0;

    virtual void setupMaterial(const Material& material, Face face = Face::Front) = 0;
    virtual void setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is) = 0;
    virtual void setupLightSpot(uint32_t index, glm::vec3 position, glm::vec3 direction, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float cutoff = 15.0f, float exponent = 20.0f) = 0;
    virtual void setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float constant = 1.0f, float linear = 0.0f, float quadratic = 0.0f) = 0;

    virtual void setLight(uint32_t index, bool enable) = 0;
    virtual void setLighting(bool enable) = 0;
    virtual void setColorMaterial(bool enable) = 0;
    virtual void setDepth(bool enable) = 0;
    virtual void setSmooth(bool enable) = 0;

    virtual bool getLight(uint32_t index) const = 0;
    virtual bool getLighting() const = 0;
    virtual bool getColorMaterial() const = 0;
    virtual bool getDepth() const = 0;
    virtual bool getSmooth() const = 0;

    virtual void setPointSize(float size) = 0;
    virtual void setLineSize(float size) = 0;

    virtual float getPointSize() const = 0;
    virtual float getLineSize() const = 0;

    virtual void setWorldMatrix(const glm::mat4& matrix) = 0;
    virtual void setProjMatrix(const glm::mat4& matrix) = 0;
    virtual void setViewMatrix(const glm::mat4& matrix) = 0;

    virtual const glm::mat4& getWorldMatrix() const = 0;
    virtual const glm::mat4& getViewMatrix() const = 0;
    virtual const glm::mat4& getProjMatrix() const = 0;
};
