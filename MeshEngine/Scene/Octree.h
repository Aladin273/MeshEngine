#pragma once

#include <memory>
#include <vector>
#include <set>
#include <algorithm>

#include "MeshEngine/RenderSystem/Shader.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"

#include "MeshEngine/Node/Node.h"

#include "MeshEngine/Math/Ray.h"
#include "MeshEngine/Math/BoundingBox.h"

class Octree 
{
public:
    Octree(const BoundingBox& bounds, uint32_t maxDepth, uint32_t maxObjects);
    ~Octree();

    void insert(Node* node);
    void remove(Node* node);
    void update(Node* node);

    void render(RenderSystem* renderSystem);

    std::vector<Node*> raycast(const Ray& ray);

private:
    void split();
    void merge();
    void query(const Ray& ray, std::vector<Node*>& results);

private:
    bool m_leaf = true;
    uint32_t m_depth = 0;
    
    uint32_t m_maxDepth;
    uint32_t m_maxObjects;

    BoundingBox m_bounds;

    Octree* m_parent = nullptr;

    std::set<Node*> m_objects;
    std::unique_ptr<Octree> m_children[8];

private:
    Shader* m_shader = nullptr;
    RenderSystem* m_renderSystem = nullptr;
    
    uint32_t m_renderId = 0;
    bool m_renderInit = false;
};