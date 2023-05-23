#pragma once

#include <string>
#include <memory>

#include "ThirdParty/glm/glm.hpp"
#include "ThirdParty/glm/gtc/matrix_transform.hpp"
#include "ThirdParty/glm/gtc/type_ptr.hpp"
#include "ThirdParty/glm/gtx/transform.hpp"

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
    void processRecursive(Lambda lambda)
    {
        lambda(*this);

        for (auto& child : m_children)
        {
            child->processRecursive(lambda);
        }
    }

protected:
    std::string m_name = "Node";
    glm::mat4 m_transform = glm::mat4(1.0f);

    Node* m_parent = nullptr;
    std::unique_ptr<Mesh> m_mesh;
    std::vector<std::unique_ptr<Node>> m_children;
};
