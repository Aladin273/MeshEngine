#include "Scene.h"

#include <glad/gl.h>

Scene::Scene()
{
    m_name = "Scene";
    m_octree = std::make_unique<Octree>(BoundingBox(glm::vec3(-MeshEngine::Settings::bounds), glm::vec3(MeshEngine::Settings::bounds)), MeshEngine::Settings::depth, MeshEngine::Settings::objects);
}

Scene::~Scene()
{

}

void Scene::init(RenderSystem* renderSystem, AssetSystem* assetSystem, InputSystem* inputSystem)
{
    m_assetSystem = assetSystem;
    m_inputSystem = inputSystem;
    m_renderSystem = renderSystem;

    matricesUniformId = m_renderSystem->bufferUniform(0, sizeof(matricesUniform), &matricesUniform);
    lightsUniformId = m_renderSystem->bufferUniform(1, sizeof(lightsUniform), &lightsUniform);

    m_renderSystem->bufferDepth(m_depthId, m_depthTextureId, m_depthWidth, m_depthHeight);
    m_shaderDepth = MeshEngine::createShader(MeshEngine::Settings::shadersPath + "depthVertex.glsl", MeshEngine::Settings::shadersPath + "depthFragment.glsl");
}

AssetSystem* Scene::getAssetSystem() const
{
    return m_assetSystem;
}

InputSystem* Scene::getInputSystem() const
{
    return m_inputSystem;
}

RenderSystem* Scene::getRenderSystem() const
{
    return m_renderSystem;
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

void Scene::attachNode(std::unique_ptr<Node> node)
{
    if (node)
    {
        node->setParent(nullptr);
        node->setScene(this);
        node->setOctree(m_octree.get());

        if (m_running)
            node->start();

        m_nodes.push_back(std::move(node));
    }
}

void Scene::detachNode(Node* node)
{
    m_deleted = node;
}

Node* Scene::getNodeById(uint32_t id)
{
    Node* target = nullptr;
    processRecursive([&](Node& node) -> bool
        {
            if (node.getId() == id)
                target = &node;

            return true;
        });

    return target;
}

std::vector<Contact> Scene::raycast(const Ray& ray, FilterValue filterValues)
{
    std::vector<Node*> candidates;
    std::vector<Contact> contacts;

    MeshEngine::Timer timer;

    if (octreeRayCast)
    {
        candidates = m_octree->raycast(ray);
        candidates.erase(std::remove_if(candidates.begin(), candidates.end(), [](Node* node) { return !node->getChildren().empty(); }), candidates.end());
    }
    else
    {
        processRecursive([&](Node& node) -> bool
            {
                BoundingBox bbox = node.getBoundingBox();
                bbox.tranform(node.getAbsoluteTransform());

                if (glm::intersectRayAABB(ray.orig, ray.dir, bbox.getMin(), bbox.getMax()) && node.getChildren().empty())
                    candidates.push_back(&node);

                return true;
            });
    }
    
    MeshEngine::Logger::info("Raycast time : {} ms ", timer.elapsed());
    MeshEngine::Logger::info("Num of objects : {}", m_frustrumCulling.size());

    // Narrow Phase
    for (auto node : candidates)
    {
        const auto& mat = node->getAbsoluteTransform();

        if (MeshNode* meshNode = dynamic_cast<MeshNode*>(node))
        {
            const auto& table = meshNode->getMesh()->getHalfEdgeTable();

            for (auto& face : table.getFaces())
            {
                HalfEdgeHandle heh0 = face.heh;
                HalfEdgeHandle heh1 = table.next(heh0);
                HalfEdgeHandle heh2 = table.next(heh1);
                HalfEdgeHandle heh3 = table.next(heh2);

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
            glm::vec3 point = mat * glm::vec4(0.f, 0.f, 0.f, 1.f);
            contacts.push_back({ invalid, node, glm::distance(ray.orig, point), point });
        }
    }

    // Sort by distance to Camera
    std::sort(contacts.begin(), contacts.end(), [&](Contact& a, Contact& b)
        {
            return a.distance < b.distance;
        });

    return contacts;
}

bool Scene::isRunning() const
{
    return m_running;
}

void Scene::start()
{
    m_running = true;

    for (auto& node : m_nodes)
        node->start();
}

void Scene::end()
{
    m_running = false;

    for (auto& node : m_nodes)
        node->end();
}

void Scene::update(float deltaTime)
{
    requestDelete();

    matricesUniform.view = m_viewport->getCamera().calcViewMatrix();
    matricesUniform.projection = m_viewport->calcProjectionMatrix();

    for (auto& node : m_nodes)
    {
        if (node->updatable)
        {
            node->update(deltaTime);
        }
    }

    if (frustrumCulling)
    {
        float fov = m_viewport->getFov();
        m_viewport->setFov(fov * frustrumScale);
        m_frustrumCulling = m_octree->frustrumcast(m_viewport->calcFrustrum());
        m_viewport->setFov(fov);
    }
}

void Scene::render(uint32_t targetId)
{
    renderDepth(m_depthId);
    renderScene(targetId);
}

void Scene::renderDepth(uint32_t targetId)
{
    m_renderSystem->bindDepth(targetId);
    m_renderSystem->setViewport(0, 0, m_depthWidth, m_depthHeight);

    m_renderSystem->clearDepth();

    if (castShadows && lightsUniform.numDirLights)
    {
        glm::vec3 lightDirection = lightsUniform.dirLights[0].direction;
        float distance = m_viewport->getCamera().getDistanceToTarget() * 2.f;

        const glm::vec3 worldUp = { 0,1,0 };

        glm::mat4 lightProjection = glm::ortho(-distance, distance, -distance, distance, (float)m_viewport->getZNear(), (float)m_viewport->getZFar());
        glm::mat4 lightView = glm::lookAt(-lightDirection * distance, lightDirection, worldUp);

        matricesUniform.lightSpaceMatrix = lightProjection * lightView;
        m_renderSystem->bufferSubUniform(matricesUniformId, 0, sizeof(matricesUniform), &matricesUniform);

        glCullFace(GL_FRONT);

        if (frustrumCulling)
        {
            for (auto& node : m_frustrumCulling)
            {
                if (node->visible)
                {
                    node->renderEx(m_renderSystem, m_shaderDepth);
                }
            }
        }
        else
        {
            for (auto& node : m_nodes)
            {
                if (node->visible)
                {
                    node->renderEx(m_renderSystem, m_shaderDepth);
                }
            }
        }

        glCullFace(GL_BACK);
    }

    m_renderSystem->unbindDepth();
}

void Scene::renderScene(uint32_t targetId)
{
    m_renderSystem->bindFrame(targetId);

    m_renderSystem->setViewport(0, 0, m_viewport->getWidth(), m_viewport->getHeight());
    m_renderSystem->clearDisplay(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

    m_renderSystem->bufferSubUniform(matricesUniformId, 0, sizeof(matricesUniform), &matricesUniform);
    m_renderSystem->bufferSubUniform(lightsUniformId, 0, sizeof(lightsUniform), &lightsUniform);

    if (frustrumCulling)
    {
        for (auto& node : m_frustrumCulling)
        {
            if (node->visible)
            {
                node->render(m_renderSystem);
            }
        }
    }
    else
    {
        for (auto& node : m_nodes)
        {
            if (node->visible)
            {
                node->render(m_renderSystem);
            }
        }
    }

    if (renderOctree)
        m_octree->render(m_renderSystem);

    m_renderSystem->unbindFrame();
}

void Scene::requestDelete()
{
    if (m_deleted)
    {
        m_deleted->detachNode();

        auto it = std::find_if(m_nodes.begin(), m_nodes.end(), [=](std::unique_ptr<Node>& candicate)
            {
                return candicate.get() == m_deleted;
            });

        if (it != m_nodes.end())
        {
            m_nodes.erase(it);
        }

        m_deleted = nullptr;
    }
}