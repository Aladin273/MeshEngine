#include "Scene.h"

#include <glad/gl.h>

Scene::Scene()
{

}

Scene::~Scene()
{

}

void Scene::init(RenderSystem* rs)
{
    m_renderSystem = rs;
    m_renderSystem->bufferDepth(m_depthId, m_depthTextureId, m_depthWidth, m_depthHeight);

    m_shaderScene.reset(MeshEngine::createShader(MeshEngine::Settings::shadersPath + "sceneVertex.glsl", MeshEngine::Settings::shadersPath + "sceneFragment.glsl"));
    m_shaderDepth.reset(MeshEngine::createShader(MeshEngine::Settings::shadersPath + "depthVertex.glsl", MeshEngine::Settings::shadersPath + "depthFragment.glsl"));
}

void Scene::setRenderSystem(RenderSystem* rs)
{
    m_renderSystem = rs;
}

RenderSystem* Scene::getRenderSystem() const
{
    return m_renderSystem;
}

void Scene::setRenderTarget(uint32_t target)
{
    m_renderTarget = target;
}

uint32_t Scene::getRenderTarget() const
{
    return m_renderTarget;
}

void Scene::setViewport(Viewport& viewport)
{
    m_viewport = &viewport;
}

Viewport& Scene::getViewport() const
{
    return *m_viewport;
}

const BoundingBox& Scene::getBoundingBox()
{
    BoundingBox bbox;
    processRecursive([&](Node& node) -> bool
        {
            BoundingBox nodebbox = node.getBoundingBox();
            nodebbox.tranform(node.getAbsoluteTransform());
            
            bbox.merge(nodebbox);
            return true;
        });

    return bbox;
}

const std::vector<std::unique_ptr<Node>>& Scene::getNodes() const
{
    return m_nodes;
}

std::vector<std::unique_ptr<Node>>& Scene::getNodes()
{
    return m_nodes;
}

void Scene::attachNode(std::unique_ptr<Node> node)
{
    if (node)
    {
        node->setParent(nullptr);
        node->setScene(this);
        node->setDirty(true);

        m_nodes.push_back(std::move(node));
    }
}

void Scene::detachNode(Node* node)
{
    m_deleted = node;
}

Node* Scene::getNodeByID(uint32_t id)
{
    Node* target = nullptr;
    processRecursive([&](Node& node) -> bool
        {
            if (node.getID() == id)
                target = &node;

            return true;
        });

    return target;
}

std::vector<Contact> Scene::raycast(const Ray& ray, FilterValue filterValues)
{
    std::vector<Node*> candidates;
    std::vector<Contact> contacts;

    // Broad Phase
    processRecursive([&](Node& node) -> bool
        {
            BoundingBox bbox = node.getBoundingBox();
            bbox.tranform(node.getAbsoluteTransform());

            if (glm::intersectAABB(ray.orig, ray.dir, bbox.min, bbox.max))
                candidates.push_back(&node);

            return true;
        });

    // Narrow Phase
    for (auto node : candidates)
    {
        const auto& mat = node->getAbsoluteTransform();

        if (MeshNode* meshNode = dynamic_cast<MeshNode*>(node))
        {
            const auto& table = meshNode->getMesh()->getHalfEdgeTable();

            for (auto& face : table.getFaces())
            {
                heds::HalfEdgeHandle heh0 = face.heh;
                heds::HalfEdgeHandle heh1 = table.next(heh0);
                heds::HalfEdgeHandle heh2 = table.next(heh1);
                heds::HalfEdgeHandle heh3 = table.next(heh2);

                float t;
                glm::vec3 a = mat * glm::vec4(table.getEndPoint(heh0).position, 1.0f);
                glm::vec3 b = mat * glm::vec4(table.getEndPoint(heh1).position, 1.0f);
                glm::vec3 c = mat * glm::vec4(table.getEndPoint(heh2).position, 1.0f);

                if (glm::intersectRayTriangle(ray.orig, ray.dir, t, a, b, c))
                    contacts.push_back({ table.handle(face), node, t, ray.orig + ray.dir * t });

                if (heh3 != heh0)
                {
                    glm::vec3 d = mat * glm::vec4(table.getEndPoint(heh3).position, 1.0f);

                    if (glm::intersectRayTriangle(ray.orig, ray.dir, t, a, c, d))
                        contacts.push_back({ table.handle(face), node, t, ray.orig + ray.dir * t });
                }
            }
        }
        else
        {
            glm::vec3 point = glm::vec4(1.f) * mat;
            contacts.push_back({ heds::invalid, node, glm::distance(ray.orig, point), point });
        }
    }

    // Filter value
    for (size_t index = 0; index < contacts.size();)
    {
        if (filterValues == FilterValue::NM)
        {
            if (dynamic_cast<Node*>(contacts[index].node) == nullptr)
            {
                contacts.erase(contacts.begin() + index);
                continue;
            }
        }
        else if (filterValues == FilterValue::Manipulator)
        {
            if (dynamic_cast<Manipulator*>(contacts[index].node) == nullptr)
            {
                contacts.erase(contacts.begin() + index);
                continue;
            }
        }
        else
        {
            if (dynamic_cast<Manipulator*>(contacts[index].node) != nullptr)
            {
                contacts.erase(contacts.begin() + index);
                continue;
            }
        }

        ++index;
    }

    // Sort by distance to Camera
    std::sort(contacts.begin(), contacts.end(), [&](Contact& a, Contact& b)
        {
            return a.distance < b.distance;
        });

    return contacts;
}

void Scene::update(float deltaTime)
{
    updateDepth(deltaTime);
    updateScene(deltaTime);
    
    requestDelete();
}

void Scene::updateDepth(float deltaTime)
{
    m_renderSystem->bindDepth(m_depthId);
    m_renderSystem->setViewport(0, 0, m_depthWidth, m_depthHeight);

    m_renderSystem->clearDepth();

    if (castShadows)
    {
        float distance = m_viewport->getCamera().getDistanceToTarget() * 2.f;

        const glm::vec3 worldUp = { 0,1,0 };

        glm::mat4 lightProjection = glm::ortho(-distance, distance, -distance, distance, (float)m_viewport->getZNear(), (float)m_viewport->getZFar());
        glm::mat4 lightView = glm::lookAt(-lightDirection * distance, lightDirection, worldUp);

        m_lightSpaceMatrix = lightProjection * lightView;

        m_shaderDepth->bind();
        m_shaderDepth->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

        glCullFace(GL_FRONT);

        processRecursive([&](Node& node) -> bool
            {
                if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
                {
                    m_shaderDepth->setMat4("model", meshNode->getAbsoluteTransform());
                    meshNode->getMesh()->render(*m_renderSystem, *m_shaderDepth);
                }

                return true;
            });

        glCullFace(GL_BACK);

        m_shaderDepth->unbind();
    }

    m_renderSystem->unbindDepth();
}

void Scene::updateScene(float deltaTime)
{
    m_renderSystem->bindFrame(m_renderTarget);

    m_renderSystem->setViewport(0, 0, m_viewport->getWidth(), m_viewport->getHeight());
    m_renderSystem->clearDisplay(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

    m_shaderScene->bind();

    m_shaderScene->setInt("depthMap", 3);
    m_renderSystem->bindTexture(3, m_depthTextureId);

    m_shaderScene->setMat4("view", m_viewport->getCamera().calcViewMatrix());
    m_shaderScene->setMat4("projection", m_viewport->calcProjectionMatrix());
    m_shaderScene->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

    m_shaderScene->setInt("numDirLights", 1);
    m_shaderScene->setInt("numPointLights", 0);
    m_shaderScene->setInt("numSpotLights", 0);

    const glm::vec3 ambient = { 0.2f, 0.2f, 0.2f };
    const glm::vec3 diffuse = { 1.f, 1.f, 1.f };
    const glm::vec3 specular = { 1.f, 1.f, 1.f };
    if (cameraLight) lightDirection = m_viewport->getCamera().calcForward();

    m_shaderScene->setVec3("dirLights[0].ambient", ambient);
    m_shaderScene->setVec3("dirLights[0].diffuse", diffuse);
    m_shaderScene->setVec3("dirLights[0].specular", specular);
    m_shaderScene->setVec3("dirLights[0].direction", lightDirection);

    processRecursive([&](Node& node) -> bool
        {
            if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
            {
                m_shaderScene->setMat4("model", meshNode->getAbsoluteTransform());
                meshNode->getMesh()->render(*m_renderSystem, *m_shaderScene);
            }

            return true;
        });

    m_shaderScene->unbind();
    m_renderSystem->unbindFrame();
}

void Scene::requestDelete()
{
    if (m_deleted)
    {
        auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [=](std::unique_ptr<Node>& candicate)
            {
                return candicate.get() == m_deleted;
            });

        if (it != m_nodes.end())
        {
            m_nodes.erase(it);
        }
        else
        {
            m_deleted->deleteFromParent();
        }

        m_deleted = nullptr;
    }
}
