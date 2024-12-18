#pragma once

#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iterator>

#include "MeshEngine/RenderSystem/Shader.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"

#include "MeshEngine/Node/Node.h"

#include "MeshEngine/Math/Ray.h"
#include "MeshEngine/Math/BoundingBox.h"

#include "MeshEngine/Viewport/Viewport.h"

class Octree 
{
public:
    Octree(const BoundingBox& bounds, uint32_t maxDepth, uint32_t maxObjects);
    ~Octree();

    void insert(Node* node, const BoundingBox& bbox);
    void remove(Node* node, const BoundingBox& bbox);
    
    void update(Node* node, const BoundingBox& bbox);
    void render(RenderSystem* renderSystem);

    std::vector<Node*> raycast(const Ray& ray);
    std::vector<Node*> frustrumcast(const std::vector<glm::vec4>& frustrum);

private:
    void split();
    void merge();
    
    void queryRay(const Ray& ray, std::unordered_map<Node*, bool>& results);
    void queryFrustrum(const std::vector<glm::vec4>& frustrum, std::unordered_map<Node*, bool>& results);

private:
    bool m_leaf = true;
    uint32_t m_depth = 0;
    
    uint32_t m_maxDepth;
    uint32_t m_maxObjects;

    BoundingBox m_bounds;

    std::unordered_set<Node*> m_objects;
    std::unordered_map<Node*, uint32_t> m_duplicates;
    
    std::unique_ptr<Octree> m_children[8];
};