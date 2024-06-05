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

double Camera::getDistanceToTarget() const
{
    return glm::length(m_target - m_eye);
}

void Camera::setDistanceToTarget(double D)
{
    m_eye = m_target - calcForward() * static_cast<float>(D);
}

void Camera::orbit(glm::vec3 a, glm::vec3 b) // Takes a and b in [-ar;1] x [ar;-1] 
{
    if (glm::length(a) >= 1.0f)  // process value outside sphere
        a = glm::normalize(a);
    else
        a.z = glm::sqrt(glm::max(0.0f, 1.0f - a.x * a.x - a.y * a.y));

    if (glm::length(b) >= 1.0f)  // process value outside sphere
        b = glm::normalize(b);
    else
        b.z = glm::sqrt(glm::max(0.0f, 1.0f - b.x * b.x - b.y * b.y));

    float dot = glm::dot(a, b);

    // Clamp dot to the valid range for acos to avoid NaN
    dot = glm::clamp(dot, -1.0f, 1.0f);

    if (dot >= 1.0f || dot <= -1.0f)
        return;

    float angle = glm::acos(dot);

    glm::vec3 axis = glm::cross(b, a);

    if (glm::length(axis) < 1e-6) // Handle the case where axis is near zero
        return;

    glm::mat3 toWorldCameraSpace = glm::transpose(glm::mat3(calcViewMatrix()));
    axis = glm::normalize(toWorldCameraSpace * axis);

    glm::mat4 orbit_matrix = glm::rotate(glm::mat4{ 1.0f }, angle, axis);

    glm::vec3 eye = glm::vec4(m_target, 1.0f) + orbit_matrix * glm::vec4(m_eye - m_target, 1.0f);
    glm::vec3 up = glm::normalize(orbit_matrix * glm::vec4(m_up, 0.0f));

    // Ensure m_eye and m_up are valid
    if (!glm::any(glm::isnan(m_eye)) && !glm::any(glm::isnan(m_up))) 
    {
        m_eye = eye;
        m_up = up;
    }
}

void Camera::orbit(glm::vec3 point, glm::vec3 a, glm::vec3 b)
{
    // Translate camera position and target point
    glm::vec3 translation = point - m_target;
    m_eye += translation;
    m_target = point;

    // Call orbit function
    orbit(a, b);

    // Translate back to original position
    m_eye -= translation;
    m_target += translation;
}

void Camera::pan(double u, double v) // Takes u and v on target plane 
{
    glm::vec3 delta = calcRight() * static_cast<float>(u) + m_up * static_cast<float>(v);
    m_eye = m_eye + delta;
    m_target = m_target + delta;
}

void Camera::look(double u, double v) // Takes u and v on target plane 
{
    glm::vec3 delta = calcRight() * static_cast<float>(u) + m_up * static_cast<float>(v);
    m_target = m_target + delta;
}

void Camera::setFrontView()
{
    double D = getDistanceToTarget();
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

void Camera::rotate(glm::vec3 point, glm::vec3 axis, double angle)
{
    translate(glm::vec3{ 0.0f } - point);
    transform(glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), axis));
    translate(point);
}

void Camera::zoom(double factor)
{
    double distance = getDistanceToTarget();
    float signFactor = static_cast<float>(factor > 1.0 ? factor : -factor);

    if (signFactor > 0)
    {
        if (distance > 1.0)
        {
            setDistanceToTarget(getDistanceToTarget() / factor);
        }
        else
        {
            if (!m_zoomEnabled)
            {
                m_zoomEnabled = true;
                m_zoomDistance = 0.0f;
            }

            glm::vec3 direction = calcForward() * signFactor;

            m_eye += direction;
            m_target += direction;

            m_zoomDistance += glm::length(direction);
        }
    }
    else
    {
        if (m_zoomEnabled && distance < 1.0)
        {
            glm::vec3 direction = calcForward() * signFactor;

            m_eye += direction;
            m_target += direction;

            m_zoomDistance -= glm::length(direction);

            if (m_zoomDistance < 0.0f)
            {
                m_zoomEnabled = false;
                setDistanceToTarget(getDistanceToTarget() / factor);
            }
        }
        else
        {
            setDistanceToTarget(getDistanceToTarget() / factor);
        }
    }
}

void Camera::transform(const glm::mat4& trf)
{
    m_eye = trf * glm::vec4{ m_eye, 1 };
    m_target = trf * glm::vec4{ m_target, 1 };
    m_up = trf * glm::vec4{ m_up, 0 };
}

void Camera::setEyeTargetUp(glm::vec3 newEye, glm::vec3 newTarget, glm::vec3 newUp)
{
    m_eye = newEye;
    m_target = newTarget;
    m_up = glm::rotate(glm::mat4(1.0f), glm::half_pi<float>(), glm::cross(calcDirection(), newUp)) * glm::vec4(calcDirection(), 1);
}
