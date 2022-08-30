#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"

struct ray
{
    glm::vec3 orig{ 0 };
    glm::vec3 dir{ 0,0,1 };
};

class Viewport
{
public:
    glm::mat4 calcProjectionMatrix() const;

    void setViewportSize(uint32_t inWidth, uint32_t inHeight);
    void setFOV(double inFOV);
    void setZNear(double inZNear);
    void setZFar(double inZFar);
    void setParallelProjection(bool use);

    double getZNear() const;
    double getZFar() const;
    double getFov() const;
    double getWidth() const;
    double getHeight() const;
    bool getParallelProjection() const;

    void zoomToFit(glm::vec3 min, glm::vec3 max);
    glm::vec3 unproject(double x, double y, double z) const;
    ray calcCursorRay(double x, double y) const;

    double calcTargetPlaneWidth() const;
    double calcTargetPlaneHeight() const;
    double calcAspectRatio() const;

    Camera& getCamera();
    const Camera& getCamera() const;

private:
    Camera m_camera;
    double m_znear = 0.01;
    double m_zfar = 500;
    double m_fov = 60;
    double m_width = 1;
    double m_height = 1;
    bool m_parallel = false;
};

