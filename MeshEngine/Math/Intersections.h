#pragma once

#include <glm/glm.hpp>

namespace glm
{
    bool intersectAABB(glm::vec3 ray_orig, glm::vec3 ray_dir, glm::vec3 bbox_min, glm::vec3 bbox_max);

    bool intersectRayTriangle(glm::vec3 ray_orig, glm::vec3 ray_dir, float& t, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);

    bool intersectRayRay(glm::vec3 a_orig, glm::vec3 a_dir, glm::vec3 b_orig, glm::vec3 b_dir, glm::vec3& point);

    bool intersectRayPlane(glm::vec3 point0, glm::vec3 normal, glm::vec3 ray_orig, glm::vec3 ray_dir, glm::vec3& point);
}
