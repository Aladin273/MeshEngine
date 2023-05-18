#pragma once

#include <vector>
#include <glm/glm.hpp>

struct Vertex
{
    glm::vec3 position{ 0.0f };
    glm::vec3 normal{ 0.0f };
    glm::vec3 color{ 1.0f };
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
    virtual void setViewport(double x, double y, double width, double height) = 0;

    virtual void renderTriangles(const std::vector<Vertex>& vertices) = 0;
    virtual void renderTriangles(const std::vector<Vertex>& vertices, glm::vec3 color) = 0;
    virtual void renderLines(const std::vector<Vertex>& vertices) = 0;
    virtual void renderLines(const std::vector<Vertex>& vertices, glm::vec3 color) = 0;

    virtual void setupMaterial(const Material& material, Face face = Face::Front) = 0;
    virtual void setupLightDir(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is) = 0;
    virtual void setupLightPoint(uint32_t index, glm::vec3 position, glm::vec3 Ia, glm::vec3 Id, glm::vec3 Is, float k_const = 1.0f, float k_linear = 0.0f, float k_quadratic = 0.0f) = 0;
    virtual void turnLight(uint32_t index, bool enable) = 0;

    virtual void setDepth(bool enable) = 0;
    virtual void setSmooth(bool enable) = 0;
    virtual void setLighting(bool enable) = 0;
    virtual void setLightTwoSide(bool enable) = 0;
    virtual void setColorMaterial(bool enable) = 0;

    virtual bool getDepth() = 0;
    virtual bool getSmooth() = 0;
    virtual bool getLighting() = 0;
    virtual bool getLightTwoSide() = 0;
    virtual bool getColorMaterial() = 0;
    virtual bool turnedLight(uint32_t index) = 0;

    virtual void setWorldMatrix(const glm::mat4& matrix) = 0;
    virtual void setProjMatrix(const glm::mat4& matrix) = 0;
    virtual void setViewMatrix(const glm::mat4& matrix) = 0;

    virtual const glm::mat4& getWorldMatrix() const = 0;
    virtual const glm::mat4& getViewMatrix() const = 0;
    virtual const glm::mat4& getProjMatrix() const = 0;
};
