#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "MeshEngine/Base/Base.h"
#include "MeshEngine/Math/BoundingBox.h"

class RenderSystem;
class Shader;

class Node : public Base
{
public:
    friend class Scene;
    friend class Octree;

public:
    Node();
    virtual ~Node();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::MatrixEx, "Transform", m_relative);

        bindProperty(updatable);
        bindProperty(visible);

        super::bind();
    }

    virtual void propertyChanged(const Property& property) override;

    bool updatable = true;
    bool visible = true;

public:
    Node* getRoot() const;
    Node* getParent() const;
    Scene* getScene() const;
    Octree* getOctree() const;
    const std::vector<std::unique_ptr<Node>>& getChildren() const;

public:
    virtual const BoundingBox& getBoundingBox() const;

public:
    Shader* getShader() const;
    void setShader(Shader* shader);

public:
    void setRelativeTransform(const glm::mat4& trf);
    const glm::mat4& getRelativeTransform();

    void setAbsoluteTransform(const glm::mat4& trf);
    const glm::mat4& getAbsoluteTransform();

    void applyRelativeTransform(const glm::mat4& trf);
    void applyAbsoluteTransform(const glm::mat4& trf);

public:
    virtual void start();
    virtual void end();

    virtual void update(float deltaTime);
    virtual void render(RenderSystem* renderSystem);
    
    virtual void renderEx(RenderSystem* renderSystem, Shader* shader);

    virtual void reset();

public:
    void attachNode(std::unique_ptr<Node> node);
    void detachNode();

    template<class Lambda>
    bool processRecursive(Lambda lambda)
    {
        if (!lambda(*this))
        {
            return false; // Stop processing this branch, but continue others
        }

        for (auto& child : m_children)
        {
            child->processRecursive(lambda);
        }
    }

protected:
    void setParent(Node* parent);
    void setScene(Scene* scene);
    void setOctree(Octree* octree);

    Octree* getOctant();
    void setOctant(Octree* octant);

    bool getDirty() const;
    void setDirty(bool dirty);

protected:
    Shader* m_shader = nullptr;

protected:
    void startBbox();
    void endBbox();

    void updateBbox(float deltaTime);
    void renderBbox(RenderSystem* renderSystem);
    
    void resetBbox();

private:
    Shader* m_shaderBase = nullptr;
    uint32_t m_renderBaseId = 0;

private:
    BoundingBox m_bbox;

    bool m_dirty = true;
    glm::mat4 m_absolute{ 1.0f };
    glm::mat4 m_relative{ 1.0f };

    Node* m_parent = nullptr;
    Scene* m_scene = nullptr;
    Octree* m_octree = nullptr;
    Octree* m_octant = nullptr;
    std::vector<std::unique_ptr<Node>> m_children;
};
