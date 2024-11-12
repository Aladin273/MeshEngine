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
    void setRelativeTransform(const glm::mat4& trf);
    const glm::mat4& getRelativeTransform();

    void setAbsoluteTransform(const glm::mat4& trf);
    const glm::mat4& getAbsoluteTransform();

    void applyRelativeTransform(const glm::mat4& trf);
    void applyAbsoluteTransform(const glm::mat4& trf);

public:
    void attachNode(std::unique_ptr<Node> node);
    void deleteFromParent();

    template<class Lambda>
    bool processRecursive(Lambda lambda)
    {
        if (!lambda(*this))
        {
            return false; // Stop processing this branch but continue others
        }

        for (auto& child : m_children)
        {
            child->processRecursive(lambda);
        }
    }

protected:
    void setParent(Node* parent);
    void setScene(Scene* scene);

    bool getDirty() const;
    void setDirty(bool dirty, bool recursive = true);

protected:
    BoundingBox m_bbox;

    bool m_dirty = true;
    glm::mat4 m_absolute{ 1.0f };
    glm::mat4 m_transform{ 1.0f };

    Scene* m_scene = nullptr;
    Node* m_root = nullptr;
    Node* m_parent = nullptr;
    std::vector<std::unique_ptr<Node>> m_children;
};
