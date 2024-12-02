#pragma once

#include <vector>

#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"

struct BoundingBox
{
    glm::vec3 min{ -1.0f };
    glm::vec3 max{ 1.0f };

    void tranform(const glm::mat4& mat);
    
    void merge(const glm::vec3& point);
    void merge(const BoundingBox& bbox);

    glm::vec3 center() const;
    glm::vec3 halfSize() const;

    bool contains(const BoundingBox& other) const;

    bool intersects(const BoundingBox& other) const;
    bool intersects(const glm::vec3& rayOrig, const glm::vec3& rayDir) const;
    bool intersects(const std::vector<glm::vec4>& frustumPlanes) const;

    bool operator==(const BoundingBox& bbox)
    {
        return min == bbox.min && max == bbox.max;
    }

    bool operator!=(const BoundingBox& bbox)
    {
        return min != bbox.min || max != bbox.max;
    }
};