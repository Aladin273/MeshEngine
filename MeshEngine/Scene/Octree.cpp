#include "Octree.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"
#include "MeshEngine/Misc/DrawHelper.h"

Octree::Octree(const BoundingBox& bounds, uint32_t maxDepth, uint32_t maxObjects)
    : m_bounds(bounds), m_maxDepth(maxDepth), m_maxObjects(maxObjects)
{
    m_objects.reserve(m_maxObjects);
    m_duplicates.reserve(m_maxObjects);
}

Octree::~Octree()
{

}

void Octree::insert(Node* node, const BoundingBox& bbox)
{
    if (!m_bounds.intersects(bbox))
        return;

    if (m_bounds.contains(bbox))
        node->setOctant(this);

    if (m_leaf)
    {
        m_objects.insert(node);

        if (m_objects.size() > m_maxObjects  && m_depth < m_maxDepth) 
        {
            split();
        }
    }
    else 
    {
        for (auto& child : m_children) 
        {
            if (child) 
            {
                child->insert(node, bbox);
            }
        }
    }
}

void Octree::remove(Node* node, const BoundingBox& bbox)
{
    if (m_leaf)
    {
        m_objects.erase(node);
    }
    else 
    {
        for (auto& child : m_children) 
        {
            if (child) 
            {
                child->remove(node, bbox);
            }
        }

        merge();
    }
}

void Octree::update(Node* node, const BoundingBox& bbox)
{
    auto octant = node->getOctant() && node->getOctant()->m_bounds.contains(bbox) ? node->getOctant() : this;

    if (!octant->m_leaf || octant->m_depth == 0)
    {
        octant->remove(node, bbox);
        octant->insert(node, bbox);
    }
}

void Octree::render(RenderSystem* renderSystem)
{
    MeshEngine::DrawHelper::drawDebugBox(renderSystem, glm::mat4(1.f), m_bounds.getMin(), m_bounds.getMax(), glm::vec4(1.f, 0.5f, 0.5f, 1.f), false, 2.f);

    for (auto& child : m_children)
    {
        if (child && !m_leaf)
        {
            child->render(renderSystem);
        }
    }
}

std::vector<Node*> Octree::raycast(const Ray& ray)
{
    std::unordered_map<Node*, bool> results;
    queryRay(ray, results);

    std::vector<Node*> filteredResults;
    filteredResults.reserve(results.size());

    for (const auto& [node, intersects] : results)
    {
        if (intersects)
        {
            filteredResults.push_back(node);
        }
    }

    return filteredResults;
}

std::vector<Node*> Octree::frustrumcast(const std::vector<glm::vec4>& frustrum)
{
    std::unordered_map<Node*, bool> results;
    queryFrustrum(frustrum, results);

    std::vector<Node*> filteredResults;
    filteredResults.reserve(results.size());

    for (const auto& [node, intersects] : results)
    {
        if (intersects)
        {
            filteredResults.push_back(node);
        }
    }

    return filteredResults;
}

void Octree::split()
{
    if (!m_init)
    {
        glm::vec3 min = m_bounds.getMin();
        glm::vec3 max = m_bounds.getMax();
        glm::vec3 center = m_bounds.getCenter();

        BoundingBox childBounds[8] =
        {
            BoundingBox(min, center),
            BoundingBox(glm::vec3(center.x, min.y, min.z), glm::vec3(max.x, center.y, center.z)),
            BoundingBox(glm::vec3(min.x, center.y, min.z), glm::vec3(center.x, max.y, center.z)),
            BoundingBox(glm::vec3(center.x, center.y, min.z), glm::vec3(max.x, max.y, center.z)),
            BoundingBox(glm::vec3(min.x, min.y, center.z), glm::vec3(center.x, center.y, max.z)),
            BoundingBox(glm::vec3(center.x, min.y, center.z), glm::vec3(max.x, center.y, max.z)),
            BoundingBox(glm::vec3(min.x, center.y, center.z), glm::vec3(center.x, max.y, max.z)),
            BoundingBox(center, max)
        };

        for (uint8_t i = 0; i < 8; ++i)
        {
            m_children[i] = std::make_unique<Octree>(childBounds[i], m_maxDepth, m_maxObjects);
            m_children[i]->m_depth = m_depth + 1;
        }

        m_init = true;
    }

    for (auto& object : m_objects)
    {
        BoundingBox bbox = object->getBoundingBox();
        bbox.tranform(object->getAbsoluteTransform());

        for (auto& child : m_children)
        {
            child->insert(object, bbox);
        }
    }

    m_objects.clear();
    m_leaf = false;
}

void Octree::merge()
{
    if (m_leaf) return;

    uint32_t totalCount = 0;

    for (auto& child : m_children)
    {
        if (child)
        {
            if (!child->m_leaf || child->m_objects.size() > m_maxObjects)
                return;

            for (auto& object : child->m_objects)
            {
                if (m_duplicates[object]++ == 0)
                {
                    ++totalCount;

                    if (totalCount > m_maxObjects)
                        return;
                }
            }
        }
    }

    for (auto& child : m_children)
    {
        m_objects.insert(child->m_objects.begin(), child->m_objects.end());
        child->m_objects.clear();
    }

    m_duplicates.clear();
    m_leaf = true;
}

void Octree::queryRay(const Ray& ray, std::unordered_map<Node*, bool>& results)
{
    if (!m_bounds.intersects(ray.orig, ray.dir))
    {
        return;
    }

    if (m_leaf)
    {
        for (auto& object : m_objects)
        {
            if (!results.contains(object))
            {
                BoundingBox bbox = object->getBoundingBox();
                bbox.tranform(object->getAbsoluteTransform());

                results[object] = bbox.intersects(ray.orig, ray.dir);
            }
        }
    }
    else
    {
        for (auto& child : m_children)
        {
            if (child)
            {
                child->queryRay(ray, results);
            }
        }
    }
}

void Octree::queryFrustrum(const std::vector<glm::vec4>& frustrum, std::unordered_map<Node*, bool>& results)
{
    if (!m_bounds.intersects(frustrum))
    {
        return;
    }

    if (m_leaf)
    {
        for (auto& object : m_objects)
        {
            if (!results.contains(object))
            {
                BoundingBox bbox = object->getBoundingBox();
                bbox.tranform(object->getAbsoluteTransform());

                results[object] = bbox.intersects(frustrum);
            }
        }
    }
    else
    {
        for (auto& child : m_children)
        {
            if (child)
            {
                child->queryFrustrum(frustrum, results);
            }
        }
    }
}