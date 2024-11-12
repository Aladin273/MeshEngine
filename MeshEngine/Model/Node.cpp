#include "Node.h" 
#include "Scene.h"

Node::Node()
{

}

Node::~Node()
{
}

Node* Node::getParent() const
{
    return m_parent;
}

Node* Node::getRoot() const
{
    Node* root = m_parent;

    while (root)
    {
        root = root->getParent();
    }

    return root;
}

Scene* Node::getScene() const
{
    return m_scene;
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
    return m_children;
}

const BoundingBox& Node::getBoundingBox() const
{
    return m_bbox;
}

void Node::setRelativeTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_transform = trf;
}

const glm::mat4& Node::getRelativeTransform()
{
    return m_transform;
}

void Node::setAbsoluteTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_transform = trf * glm::inverse(getAbsoluteTransform());
}

const glm::mat4& Node::getAbsoluteTransform()
{
    if (getDirty())
    {
        m_absolute = m_transform;

        if (m_parent)
        {
            m_absolute = m_parent->getAbsoluteTransform() * m_absolute;
        }

        setDirty(false, false);
    }

    return m_absolute;
}

void Node::applyRelativeTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_transform = trf * m_transform;
}

void Node::applyAbsoluteTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_transform = (trf * glm::inverse(getAbsoluteTransform())) * m_transform;
}

void Node::attachNode(std::unique_ptr<Node> node)
{
    if (node->getParent() != nullptr)
        throw;

    node->setParent(this);
    node->setScene(m_scene);
    node->setDirty(true);
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

void Node::setParent(Node* parent)
{
    m_parent = parent;
}

void Node::setScene(Scene* scene)
{
    m_scene = scene;

    for (auto& child : m_children)
    {
        child->setScene(scene);
    }
}

bool Node::getDirty() const
{
    return m_dirty;
}

void Node::setDirty(bool dirty, bool recursive /*= true*/)
{
    m_dirty = dirty;

    if (recursive)
    {
        for (auto& child : m_children)
        {
            child->setDirty(dirty);
        }
    }
}