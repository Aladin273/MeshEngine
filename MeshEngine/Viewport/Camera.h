#pragma once

#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"

class Camera
{
public:
    glm::mat4 calcViewMatrix() const;

    glm::vec3 calcForward() const;
    glm::vec3 calcDirection() const;
    glm::vec3 calcRight() const;

    const glm::vec3& getEye() const;
    const glm::vec3& getTarget() const;
    const glm::vec3& getUp() const;

    double getDistanceToTarget() const;
    void setDistanceToTarget(double D);

    void setFrontView();
    void setTopView();
    void setRearView();
    void setRightView();
    void setLeftView();
    void setBottomView();
    void setIsoView();

    void orbit(glm::vec3 a, glm::vec3 b);
    void pan(double u, double v);
    void look(double u, double v);

    void translate(glm::vec3 delta);
    void rotate(glm::vec3 point, glm::vec3 axis, double angle);
    void zoom(double factor);
    void transform(const glm::mat4& trf);
    void setEyeTargetUp(glm::vec3 newEye, glm::vec3 newTarget, glm::vec3 newUp);

private:
    glm::vec3 m_eye{ 0, 0, 1 };
    glm::vec3 m_target{ 0, 0, 0 };
    glm::vec3 m_up{ 0, 1, 0 };
};


