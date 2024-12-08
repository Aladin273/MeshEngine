#include "Octree.h"

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Settings.h"
#include "MeshEngine/Misc/DrawHelper.h"

Octree::Octree(const BoundingBox& bounds, uint32_t maxDepth, uint32_t maxObjects)
    : m_bounds(bounds), m_maxDepth(maxDepth), m_maxObjects(maxObjects)
{

}

Octree::~Octree()
{

}

void Octree::insert(Node* node)
{
    BoundingBox bbox = node->getBoundingBox();
    bbox.tranform(node->getAbsoluteTransform());

    if (!m_bounds.intersects(bbox))
    {
        return;
    }

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
                child->insert(node);
            }
        }
    }
}

void Octree::remove(Node* node)
{
    BoundingBox bbox = node->getBoundingBox();
    bbox.tranform(node->getAbsoluteTransform());

    //if (!m_bounds.intersects(bbox) && !m_objects.contains(node))
    //{
    //    return;
    //}

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
                child->remove(node);
            }
        }

        merge();
    }
}

void Octree::update(Node* node)
{
    BoundingBox bbox = node->getBoundingBox();
    bbox.tranform(node->getAbsoluteTransform());

    if (node->getOctant())
    {
        if (!node->getOctant()->m_bounds.contains(bbox))
        {
            node->getOctant()->remove(node);
            node->setOctant(nullptr);

            insert(node);
        }
        else
        {
            node->getOctant()->remove(node);
            node->getOctant()->insert(node);
        }
    }
    else
    {
        remove(node);
        insert(node);
    }
}

void Octree::render(RenderSystem* renderSystem)
{
    MeshEngine::DrawHelper::drawDebugBox(renderSystem, glm::mat4(1.f), m_bounds.getMin(), m_bounds.getMax(), glm::vec4(1.f, 0.5f, 0.5f, 1.f), false, 2.f);

    for (auto& child : m_children)
    {
        if (child)
        {
            child->render(renderSystem);
        }
    }
}

std::vector<Node*> Octree::raycast(const Ray& ray)
{
    std::set<Node*> results;
    queryRay(ray, results);

    return std::vector(results.begin(), results.end());
}

std::vector<Node*> Octree::frustrumcast(const std::vector<glm::vec4>& frustrum)
{
    std::set<Node*> results;
    queryFrustrum(frustrum, results);
    
    return std::vector(results.begin(), results.end());
}

void Octree::split()
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

    for (int i = 0; i < 8; ++i)
    {
        m_children[i] = std::make_unique<Octree>(childBounds[i], m_maxDepth, m_maxObjects);
        m_children[i]->m_depth = m_depth + 1;
        m_children[i]->m_parent = this;
    }

    for (auto& obj : m_objects)
    {
        for (auto& child : m_children)
        {
            BoundingBox bbox = obj->getBoundingBox();
            bbox.tranform(obj->getAbsoluteTransform());

            if (child->m_bounds.intersects(bbox))
            {
                child->insert(obj);
            }
        }
    }

    m_objects.clear();
    m_leaf = false;
}

void Octree::merge()
{
    if (m_leaf) return;

    std::set<Node*> totalObjects;
    
    for (auto& child : m_children)
    {
        if (child) 
        {
            if (!child->m_leaf)
                return;

            totalObjects.insert(child->m_objects.begin(), child->m_objects.end());
        }
    }

    if (totalObjects.size() <= m_maxObjects) 
    {
        m_objects.insert(totalObjects.begin(), totalObjects.end());

        for (auto& child : m_children) 
        {
            child.reset();
        }

        m_leaf = true;
    }
}

void Octree::queryRay(const Ray& ray, std::set<Node*>& results)
{
    if (!m_bounds.intersects(ray.orig, ray.dir))
    {
        return;
    }

    if (m_leaf)
    {
        for (auto& obj : m_objects)
        {
            BoundingBox bbox = obj->getBoundingBox();
            bbox.tranform(obj->getAbsoluteTransform());

            if (bbox.intersects(ray.orig, ray.dir))
            {
                results.insert(obj);
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

void Octree::queryFrustrum(const std::vector<glm::vec4>& frustrum, std::set<Node*>& results)
{
    if (!m_bounds.intersects(frustrum))
    {
        return;
    }

    if (m_leaf)
    {
        for (auto& obj : m_objects)
        {
            BoundingBox bbox = obj->getBoundingBox();
            bbox.tranform(obj->getAbsoluteTransform());

            if (bbox.intersects(frustrum))
            {
                results.insert(obj);
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