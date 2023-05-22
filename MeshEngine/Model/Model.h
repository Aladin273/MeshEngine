#pragma once

#include "Node.h"

class Model
{
public:
    Model();

    void attachNode(std::unique_ptr<Node> node);
    void detachNode(Node* node);
    const std::vector<std::unique_ptr<Node>>& getNodes() const;

    void setName(const std::string& inName);
    const std::string& getName() const;

    template<class Lambda>
    void processNodes(Lambda lambda)
    {
        for (auto& node : m_nodes)
            lambda(*node);
    }

    template<class Lambda>
    void processRecursive(Lambda lambda)
    {
        for (auto& node : m_nodes)
            node->processRecursive(lambda);
    }

private:
    std::vector<std::unique_ptr<Node>> m_nodes;
    std::string m_name = "Model";
};

