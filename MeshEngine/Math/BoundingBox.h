#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

class BoundingBox
{
public:
    BoundingBox();
    BoundingBox(const glm::vec3& min, const glm::vec3& max);

    void setMin(const glm::vec3& min);
    void setMax(const glm::vec3& max);

    const glm::vec3& getMin() const;
    const glm::vec3& getMax() const;

public:
    void tranform(const glm::mat4& mat);
    
    void merge(const glm::vec3& point);
    void merge(const BoundingBox& bbox);

    glm::vec3 getCenter() const;
    glm::vec3 getHalfSize() const;

    bool contains(const BoundingBox& other) const;

    bool intersects(const BoundingBox& other) const;
    bool intersects(const glm::vec3& rayOrig, const glm::vec3& rayDir) const;
    bool intersects(const std::vector<glm::vec4>& frustrum) const;

    bool operator==(const BoundingBox& bbox)
    {
        return m_min == bbox.m_min && m_max == bbox.m_max;
    }

    bool operator!=(const BoundingBox& bbox)
    {
        return m_min != bbox.m_min || m_max != bbox.m_max;
    }

private:
    glm::vec3 m_min{ -1.0f };
    glm::vec3 m_max{ 1.0f };

private:
    void updateCorners();
    glm::vec3 m_corners[8];
};