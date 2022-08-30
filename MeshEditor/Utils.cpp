#pragma once

#include "Utils.h"

bool Utils::intersectAABB(glm::vec3 ray_orig, glm::vec3 ray_dir, glm::vec3 bbox_min, glm::vec3 bbox_max)
{
    // Slab method
    glm::vec3 min = (bbox_min - ray_orig) / ray_dir;
    glm::vec3 max = (bbox_max - ray_orig) / ray_dir;

    glm::vec3 t1 = glm::min(min, max);
    glm::vec3 t2 = glm::max(min, max);

    float near = glm::max(glm::max(t1.x, t1.y), t1.z);
    float far = glm::min(glm::min(t2.x, t2.y), t2.z);

    return far >= near;
}

bool Utils::intersectRayTriangle(glm::vec3 ray_orig, glm::vec3 ray_dir, float& t, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
{
    //Möller - Trumbore algorithm
    float invDet, det, u, v;

    glm::vec3 v0v1 = v1 - v0;
    glm::vec3 v0v2 = v2 - v0;
    glm::vec3 pvec = glm::cross(ray_dir, v0v2);
    det = glm::dot(v0v1, pvec);

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < epsilon)
        return false;

    invDet = 1 / det;

    glm::vec3 tvec = ray_orig - v0;
    u = glm::dot(tvec, pvec) * invDet;

    if (u < 0 || u > 1)
        return false;

    glm::vec3 qvec = glm::cross(tvec, v0v1);
    v = glm::dot(ray_dir, qvec) * invDet;

    if (v < 0 || u + v > 1)
        return false;

    t = glm::dot(v0v2, qvec) * invDet;

    return true;
}

bool Utils::intersectRayRay(glm::vec3 a_orig, glm::vec3 a_dir, glm::vec3 b_orig, glm::vec3 b_dir, glm::vec3& point)
{
    // Skew lines
    glm::vec3 n = glm::cross(a_dir, b_dir);
    glm::vec3 n1 = glm::cross(a_dir, n);

    if (glm::length(n) > epsilon)
    {
        point = b_orig + (glm::dot((a_orig - b_orig), n1) / glm::dot(b_dir, n1)) * b_dir; // Point on ray b	
        return true;
    }

    return false;
}

bool Utils::intersectRayPlane(glm::vec3 point0, glm::vec3 normal, glm::vec3 ray_orig, glm::vec3 ray_dir, glm::vec3& point)
{
    // assuming vectors are all normalized
    float denom = glm::dot(normal, ray_dir);

    if (glm::abs(denom) > epsilon)
    {
        glm::vec3 p0l0 = point0 - ray_orig;
        float t = glm::dot(p0l0, normal) / denom;

        point = ray_orig + ray_dir * t;

        return (t >= 0);
    }

    return false;
}
