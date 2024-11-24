#include "Node.h" 

#include "MeshEngine/Scene/Scene.h"

static bool s_recursiveStart = true;
static bool s_recursiveEnd = true;
static bool s_recursiveUpdate = true;
static bool s_recursiveRender = true;

Node::Node()
{
    m_name = "Node";
    m_shaderBase = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "baseColorVertex.glsl", MeshEngine::Settings::shadersPath + "baseColorFragment.glsl");
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
    setTranformDirty(true);
    m_relative = trf;
}

const glm::mat4& Node::getRelativeTransform()
{
    return m_relative;
}

void Node::setAbsoluteTransform(const glm::mat4& trf)
{
    setTranformDirty(true);
    m_relative = trf * glm::inverse(getAbsoluteTransform());
}

const glm::mat4& Node::getAbsoluteTransform()
{
    if (getTranformDirty())
    {
        m_absolute = m_relative;

        if (m_parent)
        {
            m_absolute = m_parent->getAbsoluteTransform() * m_absolute;
        }

        setTranformDirty(false, false);
    }

    return m_absolute;
}

void Node::applyRelativeTransform(const glm::mat4& trf)
{
    setTranformDirty(true);
    m_relative = trf * m_relative;
}

void Node::applyAbsoluteTransform(const glm::mat4& trf)
{
    setTranformDirty(true);
    m_relative = (trf * glm::inverse(getAbsoluteTransform())) * m_relative;
}

void Node::start()
{
    startBbox();

    if (s_recursiveStart)
    {
        for (auto& child : m_children)
            child->start();
    }
}

void Node::end()
{
    endBbox();

    if (s_recursiveEnd)
    {
        for (auto& child : m_children)
            child->end();
    }
}

void Node::update(float deltaTime)
{
    if (updatable)
    {
        updateBbox(deltaTime);

        if (s_recursiveUpdate)
        {
            for (auto& child : m_children)
                child->update(deltaTime);
        }
    }
}

void Node::render(RenderSystem* renderSystem)
{
    if (visible)
    {
        renderBbox(renderSystem);

        if (s_recursiveRender)
        {
            for (auto& child : m_children)
                child->render(renderSystem);
        }
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
    node->setTranformDirty(true);
    node->start();

    m_children.push_back(std::move(node));
}

void Node::detachNode()
{
    end();

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

bool Node::getTranformDirty() const
{
    return m_transformDirty;
}

void Node::setTranformDirty(bool dirty, bool recursive /*= true*/)
{
    m_transformDirty = dirty;

    if (recursive)
    {
        for (auto& child : m_children)
        {
            child->setTranformDirty(dirty);
        }
    }
}

void Node::startBbox()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        m_bbox = getBoundingBox();

        std::vector<Vertex> vertices
        {
            { { m_bbox.min.x, m_bbox.min.y, m_bbox.min.z }, {}, {} },
            { { m_bbox.max.x, m_bbox.min.y, m_bbox.min.z }, {}, {} },
            { { m_bbox.min.x, m_bbox.max.y, m_bbox.min.z }, {}, {} },
            { { m_bbox.max.x, m_bbox.max.y, m_bbox.min.z }, {}, {} },
            { { m_bbox.min.x, m_bbox.min.y, m_bbox.max.z }, {}, {} },
            { { m_bbox.max.x, m_bbox.min.y, m_bbox.max.z }, {}, {} },
            { { m_bbox.min.x, m_bbox.max.y, m_bbox.max.z }, {}, {} },
            { { m_bbox.max.x, m_bbox.max.y, m_bbox.max.z }, {}, {} },
        };

        std::vector<uint32_t> indices
        {
            0, 1, 1, 3, 3, 2,
            2, 0, 4, 5, 5, 7,
            7, 6, 6, 4, 0, 4,
            1, 5, 2, 6, 3, 7,
        };

        RenderSystem* renderSystem = getScene()->getRenderSystem();
        m_renderBaseId = renderSystem->bufferData(vertices, indices);
    }
}

void Node::endBbox()
{
    if (getScene() && getScene()->getRenderSystem())
    {
        RenderSystem* renderSystem = getScene()->getRenderSystem();
        renderSystem->unbufferData(m_renderBaseId);
    }
}

void Node::updateBbox(float deltaTime)
{
    if (m_bbox != getBoundingBox())
    {
        resetBbox();
    }
}

void Node::renderBbox(RenderSystem* renderSystem)
{
    if (getScene() && getScene()->renderBbox)
    {
        m_shaderBase->bind();
        
        m_shaderBase->setMat4("model", getAbsoluteTransform());
        m_shaderBase->setVec4("color", 1.f, 1.f, 0.f, 1.f);

        renderSystem->bindData(m_renderBaseId);

        renderSystem->setLineSize(2.0f);
        renderSystem->renderLines();

        m_shaderBase->unbind();
        renderSystem->unbindData();
    }
}

void Node::resetBbox()
{
    endBbox();
    startBbox();
}
