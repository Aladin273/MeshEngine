#include "Viewport.h"

glm::mat4 Viewport::calcProjectionMatrix() const
{
    if (m_parallel)
    {
        double height = calcTargetPlaneHeight() / 2;
        double width = height * calcAspectRatio();

        return glm::ortho(-width, width, -height, height, m_znear, m_zfar);
    }

    double height = glm::tan(glm::radians(m_fov / 2)) * m_znear;
    double width = height * calcAspectRatio();
    return glm::frustum(-width, width, -height, height, m_znear, m_zfar);  
}

void Viewport::setViewportSize(uint32_t inWidth, uint32_t inHeight)
{
    m_width = inWidth;
    m_height = inHeight;
}

void Viewport::setFOV(double inFOV)
{
    m_fov = inFOV;
}

void Viewport::setZNear(double inZNear)
{
    m_znear = inZNear;
}

void Viewport::setZFar(double inZFar)
{
    m_zfar = inZFar;
}

void Viewport::setParallelProjection(bool use)
{
    m_parallel = use;
}

double Viewport::getZNear() const
{
    return m_znear;
}

double Viewport::getZFar() const
{
    return m_zfar;
}

double Viewport::getFov() const
{
    return m_fov;
}

double Viewport::getWidth() const
{
    return m_width;
}

double Viewport::getHeight() const
{
    return m_height;
}

bool Viewport::getParallelProjection() const
{
    return m_parallel;
}

void Viewport::zoomToFit(glm::vec3 min, glm::vec3 max)
{
    glm::vec3 center = (max + min) / 2.f;
    float width = static_cast<float>(calcTargetPlaneWidth());
    float height = static_cast<float>(calcTargetPlaneHeight());
    float length = glm::length(max - min);
    float current_length = (height < width) ? height : width;

    m_camera.translate(center - m_camera.getTarget());
    m_camera.zoom(current_length / length);
}

glm::vec3 Viewport::unproject(double x, double y, double z) const
{
    glm::vec4 point = { x, y, z, 1.0f };
    glm::mat4 proj = calcProjectionMatrix();
    glm::mat4 view = m_camera.calcViewMatrix();

    point.x = (point.x / m_width);
    point.y = (point.y / m_height);
    point = point * 2.0f - 1.0f;

    point = glm::inverse(proj * view) * point;
    point /= point.w;

    return point;
}
ray Viewport::calcCursorRay(double x, double y) const
{
    glm::vec3 a = unproject(x, y, -1.0);
    glm::vec3 b = unproject(x, y, 1.0);
    return { a, glm::normalize(b - a) };
}

double Viewport::calcTargetPlaneWidth() const
{
    return calcTargetPlaneHeight() * calcAspectRatio();
}

double Viewport::calcTargetPlaneHeight() const
{
    return 2.0 * m_camera.distanceFromEyeToTarget() * glm::tan(glm::radians(m_fov / 2.0));
}

double Viewport::calcAspectRatio() const
{
    return m_width / m_height;
}

Camera& Viewport::getCamera()
{
    return m_camera;
}

const Camera& Viewport::getCamera() const
{
    return m_camera;
}
