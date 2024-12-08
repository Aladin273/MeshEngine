#include "Node.h" 

#include "MeshEngine/Scene/Scene.h"
#include "MeshEngine/Misc/DrawHelper.h"

static bool s_recursiveStart = true;
static bool s_recursiveEnd = true;
static bool s_recursiveUpdate = true;
static bool s_recursiveRender = true;

Node::Node()
{
    m_name = "Node";
}

Node::~Node()
{

}

void Node::propertyChanged(const Property& property)
{
    setDirty(true);
}

Node* Node::getParent() const
{
    return m_parent;
}

Node* Node::getRoot() const
{
    Node* root = const_cast<Node*>(this);

    while (root->getParent())
    {
        root = root->getParent();
    }

    return root;
}

Scene* Node::getScene() const
{
    return m_scene;
}

Octree* Node::getOctree() const
{
    return m_octree;
}

const std::vector<std::unique_ptr<Node>>& Node::getChildren() const
{
    return m_children;
}

const BoundingBox& Node::getBoundingBox() const
{
    return m_bbox;
}

Shader* Node::getShader() const
{
    return m_shader;
}

void Node::setShader(Shader* shader)
{
    m_shader = shader;
}

void Node::setRelativeTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_relative = trf;
}

const glm::mat4& Node::getRelativeTransform()
{
    return m_relative;
}

void Node::setAbsoluteTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_relative = trf * glm::inverse(getAbsoluteTransform());
}

const glm::mat4& Node::getAbsoluteTransform()
{
    if (getDirty())
    {
        m_absolute = m_relative;

        if (m_parent)
        {
            m_absolute = m_parent->getAbsoluteTransform() * m_absolute;
        }

        setDirty(false);
    }

    return m_absolute;
}

void Node::applyRelativeTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_relative = trf * m_relative;
}

void Node::applyAbsoluteTransform(const glm::mat4& trf)
{
    setDirty(true);
    m_relative = (trf * glm::inverse(getAbsoluteTransform())) * m_relative;
}

void Node::start()
{
    if (s_recursiveStart)
    {
        for (auto& child : m_children)
            child->start();
    }
}

void Node::end()
{
    if (s_recursiveEnd)
    {
        for (auto& child : m_children)
            child->end();
    }
}

void Node::update(float deltaTime)
{
    if (updatable && s_recursiveUpdate)
    {
        for (auto& child : m_children)
            child->update(deltaTime);
    }
}

void Node::render(RenderSystem* renderSystem)
{
    if (getScene() && getScene()->renderBbox)
    {
        MeshEngine::DrawHelper::drawDebugBox(renderSystem, getAbsoluteTransform(), getBoundingBox().getMin(), getBoundingBox().getMax(), glm::vec4(1.f, 1.f, 0.f, 1.f), false, 2.f);
    }

    if (visible && s_recursiveRender)
    {
        for (auto& child : m_children)
            child->render(renderSystem);
    }
}

void Node::renderEx(RenderSystem* renderSystem, Shader* shader)
{
    if (visible)
    {
        s_recursiveRender = false;

        Shader* temp = m_shader;
        m_shader = shader;
        render(renderSystem);
        m_shader = temp;

        s_recursiveRender = true;

        for (auto& child : m_children)
        {
            child->renderEx(renderSystem, shader);
        }
    }
}

void Node::reset()
{
    end();
    start();
}

void Node::attachNode(std::unique_ptr<Node> node)
{
    if (node->getParent() != nullptr)
        throw;

    node->setParent(this);
    node->setScene(m_scene);
    node->setOctree(m_octree);
    
    node->start();
    m_children.push_back(std::move(node));
}

void Node::detachNode()
{
    end();

    if (m_parent)
    {
        m_parent->m_children.erase(std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(), [&](std::unique_ptr<Node>& node) { return node.get() == this; }));
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

void Node::setOctree(Octree* octree)
{
    m_octree = octree;

    for (auto& child : m_children)
    {
        child->setOctree(octree);
    }
}

void Node::setOctant(Octree* octant)
{
    m_octant = octant;
}

Octree* Node::getOctant()
{
    return m_octant;
}

bool Node::getDirty() const
{
    return m_dirty;
}

void Node::setDirty(bool dirty)
{
    m_dirty = dirty;

    if (dirty)
    {
        if (getOctree())
            getOctree()->update(this);

        for (auto& child : m_children)
        {
            child->setDirty(dirty);
        }
    }
}