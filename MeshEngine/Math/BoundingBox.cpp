#include "BoundingBox.h"

BoundingBox::BoundingBox()
{
    updateCorners();
}

BoundingBox::BoundingBox(const glm::vec3& min, const glm::vec3& max)
{
    m_min = min;
    m_max = max;
    updateCorners();
}
void BoundingBox::setMin(const glm::vec3& min)
{
    m_min = min;
    updateCorners();
}

void BoundingBox::setMax(const glm::vec3& max)
{
    m_max = max;
    updateCorners();
}

const glm::vec3& BoundingBox::getMin() const
{
    return m_min;
}

const glm::vec3& BoundingBox::getMax() const
{
    return m_max;
}

void BoundingBox::tranform(const glm::mat4& mat)
{
    m_min = mat * glm::vec4(m_min, 1.0f);
    m_max = mat * glm::vec4(m_max, 1.0f);

    updateCorners();
}

void BoundingBox::merge(const glm::vec3& point)
{
    m_min = glm::min(m_min, point);
    m_max = glm::max(m_max, point);

    updateCorners();
}

void BoundingBox::merge(const BoundingBox& bbox)
{
    m_min = glm::min(m_min, bbox.m_min);
    m_max = glm::max(m_max, bbox.m_max);

    updateCorners();
}

glm::vec3 BoundingBox::getCenter() const 
{
    return (m_min + m_max) * 0.5f;
}

glm::vec3 BoundingBox::getHalfSize() const
{
    return (m_max - m_min) * 0.5f;
}

bool BoundingBox::contains(const BoundingBox& other) const
{
    return (other.m_min.x >= m_min.x && other.m_max.x <= m_max.x) &&
        (other.m_min.y >= m_min.y && other.m_max.y <= m_max.y) &&
        (other.m_min.z >= m_min.z && other.m_max.z <= m_max.z);
}

bool BoundingBox::intersects(const BoundingBox& other) const
{
    return (m_min.x <= other.m_max.x && m_max.x >= other.m_min.x) &&
        (m_min.y <= other.m_max.y && m_max.y >= other.m_min.y) &&
        (m_min.z <= other.m_max.z && m_max.z >= other.m_min.z);
}

bool BoundingBox::intersects(const glm::vec3& rayOrig, const glm::vec3& rayDir) const
{
    glm::vec3 temp_min = (m_min - rayOrig) / rayDir;
    glm::vec3 temp_max = (m_max - rayOrig) / rayDir;

    glm::vec3 t1 = glm::min(temp_min, temp_max);
    glm::vec3 t2 = glm::max(temp_min, temp_max);

    float near = glm::max(glm::max(t1.x, t1.y), t1.z);
    float far = glm::min(glm::min(t2.x, t2.y), t2.z);

    return far >= near;
}

bool BoundingBox::intersects(const std::vector<glm::vec4>& frustrum) const
{
    for (const auto& plane : frustrum)
    {
        bool allOutside = true;
    
        for (const auto& corner : m_corners)
        {
            float distance = glm::dot(glm::vec3(plane), corner) + plane.w;
            if (distance >= 0)
            {
                allOutside = false;
                break;
            }
        }
    
        if (allOutside)
        {
            return false;
        }
    }
    
    return true;
}

void BoundingBox::updateCorners()
{
    m_corners[0].x = m_min.x; m_corners[0].y = m_min.y; m_corners[0].z = m_min.z;
    m_corners[1].x = m_min.x; m_corners[1].y = m_min.y; m_corners[1].z = m_max.z;
    m_corners[2].x = m_min.x; m_corners[2].y = m_max.y; m_corners[2].z = m_min.z;
    m_corners[3].x = m_min.x; m_corners[3].y = m_max.y; m_corners[3].z = m_max.z;
    m_corners[4].x = m_max.x; m_corners[4].y = m_min.y; m_corners[4].z = m_min.z;
    m_corners[5].x = m_max.x; m_corners[5].y = m_min.y; m_corners[5].z = m_max.z;
    m_corners[6].x = m_max.x; m_corners[6].y = m_max.y; m_corners[6].z = m_min.z;
    m_corners[7].x = m_max.x; m_corners[7].y = m_max.y; m_corners[7].z = m_max.z;
}