#pragma once

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

class Camera
{
public:
    glm::mat4 calcViewMatrix() const;

    glm::vec3 calcForward() const;
    glm::vec3 calcDirection() const;
    glm::vec3 calcRight() const;
    float distanceFromEyeToTarget() const;
    const glm::vec3& getEye() const;
    const glm::vec3& getTarget() const;
    const glm::vec3& getUp() const;

    void setFrontView();
    void setTopView();
    void setRearView();
    void setRightView();
    void setLeftView();
    void setBottomView();
    void setIsoView();

    void zoom(float factor);
    void orbit(glm::vec3 a, glm::vec3 b);
    void pan(float u, float v);

    void translate(glm::vec3 delta);
    void transform(const glm::mat4& trf);
    void rotate(glm::vec3 point, glm::vec3 axis, float angle);
    void setDistanceToTarget(float D);
    void setEyeTargetUp(glm::vec3 newEye, glm::vec3 newTarget, glm::vec3 newUp);

private:

    glm::vec3 m_eye{ 0, 0, 1 };
    glm::vec3 m_target{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
};


