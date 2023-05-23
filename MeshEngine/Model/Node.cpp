#include "Node.h" 

Node::Node()
{
    m_mesh = std::make_unique<Mesh>(heds::HalfEdgeTable()); // empty mesh for valid
}

Node::~Node()
{
}

void Node::setName(const std::string& name)
{
    m_name = name;
}

const std::string& Node::getName() const
{
    return m_name;
}

void Node::attachMesh(std::unique_ptr<Mesh> mesh)
{
    m_mesh = std::move(mesh);
}

Mesh* Node::getMesh() const
{
    return m_mesh.get();
}

void Node::setParent(Node* parent)
{
    m_parent = parent;
}

Node* Node::getParent() const
{
    return m_parent;
}

void Node::setRelativeTransform(const glm::mat4& trf)
{
    m_transform = trf;
}

const glm::mat4& Node::getRelativeTransform() const
{
    return m_transform;
}

void Node::applyRelativeTransform(const glm::mat4& trf)
{
    m_transform = trf * m_transform;
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
    return m_children;
}

glm::mat4 Node::calcAbsoluteTransform() const
{
    Node* parent = m_parent;
    glm::mat4 absolute = m_transform;

    while (parent != nullptr)
    {
        absolute = parent->getRelativeTransform() * absolute;
        parent = parent->getParent();
    }

    return absolute;
}

void Node::attachNode(std::unique_ptr<Node> node)
{
    if (node->getParent() != nullptr)
        throw;

    node->setParent(this);
    m_children.push_back(std::move(node));
}

void Node::deleteFromParent()
{
    if (m_parent)
    {
        m_parent->m_children.erase(std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(), [&](std::unique_ptr<Node>& node)
        {
            return node.get() == this;
        }));
    }
}
