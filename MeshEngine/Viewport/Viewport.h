#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

#include "MeshEngine/Math/Ray.h"

class Viewport
{
public:
    glm::mat4 calcProjectionMatrix() const;

    void setViewportSize(uint32_t inWidth, uint32_t inHeight);
    void setFov(double inFov);
    void setZNear(double inZNear);
    void setZFar(double inZFar);
    void setOrthogonal(bool enable);

    double getZNear() const;
    double getZFar() const;
    double getFov() const;
    double getWidth() const;
    double getHeight() const;
    bool getOrthogonal() const;

    void zoomToFit(glm::vec3 min, glm::vec3 max);
    
    glm::vec3 unproject(double x, double y) const;
    glm::vec3 unproject(double x, double y, double z) const;
    
    Ray calcCursorRay(double x, double y) const;
    Ray calcEyeRay(double x, double y) const;

    double calcTargetPlaneWidth() const;
    double calcTargetPlaneHeight() const;
    double calcAspectRatio() const;
    
    std::vector<glm::vec4> calcFrustrum() const;

    Camera& getCamera();
    const Camera& getCamera() const;

private:
    Camera m_camera;
    double m_znear = 0.1;
    double m_zfar = 1000;
    double m_fov = 60;
    double m_width = 1;
    double m_height = 1;
    bool m_orthogonal = false;
};

