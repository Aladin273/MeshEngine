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

public:
    Node();
    virtual ~Node();

public:
    Node* getParent() const;
    Node* getRoot() const;
    Scene* getScene() const;
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

    bool getTranformDirty() const;
    void setTranformDirty(bool dirty, bool recursive = true);

protected:
    BoundingBox m_bbox;
    Shader* m_shader = nullptr;

    bool m_transformDirty = true;
    glm::mat4 m_absolute{ 1.0f };
    glm::mat4 m_relative{ 1.0f };

    Scene* m_scene = nullptr;
    Node* m_root = nullptr;
    Node* m_parent = nullptr;
    std::vector<std::unique_ptr<Node>> m_children;
};
