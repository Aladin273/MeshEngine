#include "Camera.h"

glm::mat4 Camera::calcViewMatrix() const
{
    return glm::lookAt(m_eye, m_target, m_up);
}

glm::vec3 Camera::calcForward() const
{
    return glm::normalize(m_target - m_eye);
}

glm::vec3 Camera::calcDirection() const
{
    return glm::normalize(m_eye - m_target);
}

glm::vec3 Camera::calcRight() const
{
    return glm::normalize(glm::cross(m_up, calcDirection()));
}

float Camera::distanceFromEyeToTarget() const
{
    return glm::length(m_target - m_eye);
}

const glm::vec3& Camera::getEye() const
{
    return m_eye;
}

const glm::vec3& Camera::getTarget() const
{
    return m_target;
}

const glm::vec3& Camera::getUp() const
{
    return m_up;
}

void Camera::zoom(float factor)
{
    setDistanceToTarget(distanceFromEyeToTarget() / factor);
}

void Camera::orbit(glm::vec3 a, glm::vec3 b) // Takes a and b in [-ar;1] x [ar;-1] 
{
    float radius = 1.0f;

    if (glm::length(a) >= radius)  // process value outside sphere
        a = glm::normalize(a);
    else
        a.z = glm::sqrt(radius - a.x * a.x - a.y * a.y);

    if (glm::length(b) >= radius)  // process value outside sphere
        b = glm::normalize(b);
    else
        b.z = glm::sqrt(radius - b.x * b.x - b.y * b.y);

    float dot = glm::dot(a, b);

    if (dot >= 1.0f)
        return;
    if (dot <= -1.0f)
        return;

    float angle = glm::acos(dot);

    glm::vec3 axis = glm::cross(b, a);

    glm::mat3 toWorldCameraSpace = glm::transpose(glm::mat3(calcViewMatrix()));
    axis = glm::normalize(toWorldCameraSpace * axis);

    glm::mat4 orbit_matrix = glm::rotate(glm::mat4{ 1.0f }, angle, axis);

    m_eye = glm::vec4(m_target, 1.0f) + orbit_matrix * glm::vec4(m_eye - m_target, 1.0f);
    m_up = glm::normalize(orbit_matrix * glm::vec4(m_up, 0.0f));
}

void Camera::pan(float u, float v) // Takes u and v on target plane 
{
    glm::vec3 sum = calcRight() * u + m_up * v;

    m_eye = m_eye + sum;
    m_target = m_target + sum;
}

void Camera::setFrontView()
{
    float D = distanceFromEyeToTarget();
    setEyeTargetUp(m_target + glm::vec3{ 0,0,1 }, m_target, { 0,1,0 });
    setDistanceToTarget(D);
}

void Camera::setRightView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 0,1,0 }, glm::half_pi<float>());
}

void Camera::setLeftView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 0,1,0 }, -glm::half_pi<float>());
}

void Camera::setRearView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 0,1,0 }, glm::pi<float>());
}

void Camera::setTopView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 1,0,0 }, -glm::half_pi<float>());
}

void Camera::setBottomView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 1,0,0 }, glm::half_pi<float>());
}

void Camera::setIsoView()
{
    glm::vec3 oldTarget = m_target;
    setFrontView();
    rotate(oldTarget, { 1,0,0 }, -glm::quarter_pi<float>());
    rotate(oldTarget, { 0,1,0 }, glm::quarter_pi<float>());
}

void Camera::translate(glm::vec3 delta)
{
    m_eye += delta;
    m_target += delta;
}

void Camera::setDistanceToTarget(float D)
{
    m_eye = m_target - calcForward() * D;
}

void Camera::transform(const glm::mat4& transform)
{
    m_eye = transform * glm::vec4{ m_eye, 1 };
    m_target = transform * glm::vec4{ m_target, 1 };
    m_up = transform * glm::vec4{ m_up, 0 };
}

void Camera::rotate(glm::vec3 point, glm::vec3 axis, float angle)
{
    translate(glm::vec3{ 0.0f } - point);
    transform(glm::rotate(glm::mat4(1.0f), angle, axis));
    translate(point);
}

void Camera::setEyeTargetUp(glm::vec3 newEye, glm::vec3 newTarget, glm::vec3 newUp)
{
    m_eye = newEye;
    m_target = newTarget;
    m_up = glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::cross(calcDirection(), newUp)) * glm::vec4(calcDirection(), 1);
}
