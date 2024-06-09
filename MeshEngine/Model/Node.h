#pragma once

#include <string>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

#include "Mesh.h"

class Node
{
public:
    Node();
    virtual ~Node();

    void setName(const std::string& name);
    const std::string& getName() const;

    void attachMesh(std::unique_ptr<Mesh> mesh);
    Mesh* getMesh() const;

    void setParent(Node* parent);
    Node* getParent() const;

    void setRelativeTransform(const glm::mat4& trf);
    const glm::mat4& getRelativeTransform() const;

    void applyRelativeTransform(const glm::mat4& trf);

    const std::vector<std::unique_ptr<Node>>& getChildren() const;

    glm::mat4 calcAbsoluteTransform() const;

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
    std::string m_name = "Node";
    glm::mat4 m_transform{ 1.0f };

    Node* m_parent = nullptr;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<std::unique_ptr<Node>> m_children;
};
