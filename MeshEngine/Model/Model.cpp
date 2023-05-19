#include "Model.h"

Model::Model()
{
}

void Model::attachNode(std::unique_ptr<Node> node)
{
    m_nodes.push_back(std::move(node));
}

void Model::detachNode(Node* inNode)
{
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [=](std::unique_ptr<Node>& node)
        {
            return node.get() == inNode;
        });

    if (it != m_nodes.end())
        m_nodes.erase(it);
}

const std::vector<std::unique_ptr<Node>>& Model::getNodes() const
{
    return m_nodes;
}

void Model::setName(const std::string& inName)
{
    m_name = inName;
}

const std::string& Model::getName() const
{
    return m_name;
}
