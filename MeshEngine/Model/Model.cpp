#include "Model.h"

Model::Model()
{
}

Model::~Model()
{

}

void Model::setName(const std::string& name)
{
    m_name = name;
}

const std::string& Model::getName() const
{
    return m_name;
}

const std::vector<std::unique_ptr<Node>>& Model::getNodes() const
{
    return m_nodes;
}

std::vector<std::unique_ptr<Node>>& Model::getNodes()
{
    return m_nodes;
}

void Model::attachNode(std::unique_ptr<Node> node)
{
    m_nodes.push_back(std::move(node));
}

void Model::detachNode(Node* node)
{
    auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [=](std::unique_ptr<Node>& candicate)
        {
            return node == candicate.get();
        });

    if (it != m_nodes.end())
        m_nodes.erase(it);
}