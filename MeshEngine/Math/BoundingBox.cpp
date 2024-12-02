#include "BoundingBox.h"

void BoundingBox::tranform(const glm::mat4& mat)
{
    min = mat * glm::vec4(min, 1.0f);
    max = mat * glm::vec4(max, 1.0f);
}

void BoundingBox::merge(const glm::vec3& point)
{
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void BoundingBox::merge(const BoundingBox& bbox)
{
    min = glm::min(min, bbox.min);
    max = glm::max(max, bbox.max);
}

glm::vec3 BoundingBox::center() const 
{
    return (min + max) * 0.5f;
}

glm::vec3 BoundingBox::halfSize() const
{
    return (max - min) * 0.5f;
}

bool BoundingBox::contains(const BoundingBox& other) const
{
    return (other.min.x >= min.x && other.max.x <= max.x) &&
        (other.min.y >= min.y && other.max.y <= max.y) &&
        (other.min.z >= min.z && other.max.z <= max.z);
}

bool BoundingBox::intersects(const BoundingBox& other) const
{
    return (min.x <= other.max.x && max.x >= other.min.x) &&
        (min.y <= other.max.y && max.y >= other.min.y) &&
        (min.z <= other.max.z && max.z >= other.min.z);
}

bool BoundingBox::intersects(const glm::vec3& rayOrig, const glm::vec3& rayDir) const
{
    glm::vec3 tempMin = (min - rayOrig) / rayDir;
    glm::vec3 tempMax = (max - rayOrig) / rayDir;

    glm::vec3 t1 = glm::min(tempMin, tempMax);
    glm::vec3 t2 = glm::max(tempMin, tempMax);

    float near = glm::max(glm::max(t1.x, t1.y), t1.z);
    float far = glm::min(glm::min(t2.x, t2.y), t2.z);

    return far >= near;
}