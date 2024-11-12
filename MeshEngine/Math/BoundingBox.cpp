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