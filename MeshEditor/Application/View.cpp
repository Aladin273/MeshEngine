#include "View.h"
#include "Application.h"

#include <glad/gl.h>

View::View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    m_window.reset(Application::instance()->createWindow(title, width, height, icon));
    m_guiSystem.reset(Application::instance()->createGuiSystem(m_window.get()));

    m_guiSystem->init();

    m_renderSystem = rs;
    m_renderSystem->init();
    m_renderSystem->setViewport(Settings::x, Settings::y, width, height);
    
    m_renderSystem->bufferFrame(m_frameId, m_frameRenderId, m_frameTextureId, width, height);
    m_renderSystem->bufferDepth(m_depthId, m_depthTextureId, m_depthWidth, m_depthHeight);
    
    m_shader = Application::instance()->createShader(Settings::shadersPath + "vertex.glsl", Settings::shadersPath + "fragment.glsl");
    m_shaderDepth = Application::instance()->createShader(Settings::shadersPath + "vertexDepth.glsl", Settings::shadersPath + "fragmentDepth.glsl");

    m_dockpaneLayer = std::make_unique<DockpaneLayer>(this);
    m_consoleLayer = std::make_unique<ConsoleLayer>(this);
    m_propertiesLayer = std::make_unique<PropertiesLayer>(this);
    m_treeLayer = std::make_unique<TreeLayer>(this);
    m_viewportLayer = std::make_unique<ViewportLayer>(this);
    m_settingsLayer = std::make_unique<SettingsLayer>(this);

    m_viewport.getCamera().setEyeTargetUp(Settings::eye, Settings::target, Settings::up);
    m_viewport.setViewportSize(width, height);
    m_viewport.setFOV(Settings::fov);
    m_viewport.setZNear(Settings::znear);
    m_viewport.setZFar(Settings::zfar);

    m_plane = std::make_unique<Node>();
    m_origin = std::make_unique<Triad>();

    decoratePlane(*m_plane);
    decorateOrigin(*m_origin);

    m_window->setKeyCallback([&](KeyCode key, Action action, Modifier mods)
        {
            if (m_guiSystem->wantCaptureKeyboard()) return;

            m_operatorDispatcher.processKeyboardInput(*this, key, action, mods);
        });

    m_window->setMouseCallback([&](ButtonCode button, Action action, Modifier mods, double x, double y)
        {
            if (m_guiSystem->wantCaptureMouse() && m_viewportLayer->wantCaptureMouse())
            {
                m_viewportLayer->remapToRelative(x, y);
                m_operatorDispatcher.processMouseInput(*this, button, action, mods, x, y);
            }
        });

    m_window->setCursorPosCallback([&](double x, double y)
        {
            if (m_guiSystem->wantCaptureMouse() && m_viewportLayer->wantCaptureMouse())
            {
                m_viewportLayer->remapToRelative(x, y);
                m_operatorDispatcher.processMouseMove(*this, x, y);
            }
        });

    m_window->setScrollCallback([&](double x, double y)
        {
            if (m_viewportLayer->wantCaptureMouse())
            {
                double factor = y > Settings::invalid ? Settings::zoomIn : Settings::zoomOut;
                m_viewport.getCamera().zoom(factor);
            }
        });

    m_window->setFramebufferSizeCallback([&](int width, int height)
        {
            //m_viewport.setViewportSize(width, height);
        });

    m_viewportLayer->setFramebufferSizeCallback([&](int width, int height)
        {
            m_viewport.setViewportSize(width, height);

            m_renderSystem->unbufferFrame(m_frameId);
            m_renderSystem->bufferFrame(m_frameId, m_frameRenderId, m_frameTextureId, width, height);

            m_viewportLayer->attach(m_frameTextureId);
        });
}

View::~View()
{
    m_window.reset();
}

void View::update()
{
    updateModel();
    updateShadows();
}

void View::updateModel()
{
    m_renderSystem->bindFrame(m_frameId);

    m_renderSystem->setViewport(Settings::x, Settings::y, m_viewport.getWidth(), m_viewport.getHeight());
    m_renderSystem->clearDisplay(backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);

    m_shader->bind();
    m_shader->setBool("flatShading", flatShading);

    m_shader->setInt("depthMap", 3);
    m_renderSystem->bindTexture(3, m_depthTextureId);

    m_shader->setMat4("view", m_viewport.getCamera().calcViewMatrix());
    m_shader->setMat4("projection", m_viewport.calcProjectionMatrix());
    m_shader->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

    m_shader->setInt("numDirLights", 1);
    m_shader->setInt("numPointLights", 0);
    m_shader->setInt("numSpotLights", 0);

    if (cameraLight)
        lightDirection = m_viewport.getCamera().calcForward();

    m_shader->setVec3("dirLights[0].direction", lightDirection);
    m_shader->setVec3("dirLights[0].ambient", Settings::ambient);
    m_shader->setVec3("dirLights[0].diffuse", Settings::diffuse);
    m_shader->setVec3("dirLights[0].specular", Settings::specular);

    // Plane render
    if (showPlane)
    {
        m_plane->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget())));
        m_shader->setMat4("model", m_plane->calcAbsoluteTransform());
        m_plane->getMesh()->render(*m_renderSystem, *m_shader);
    }

    // Selected Render
    if (getSelected())
    {
        m_shader->setVec3("outline", 0.2f, 0.2f, 0.2f);
        
        getSelected()->processRecursive([&](Node& node) -> bool
            {
                m_shader->setMat4("model", node.calcAbsoluteTransform());
                node.getMesh()->render(*m_renderSystem, *m_shader);

                return true;
            });

        m_shader->setVec3("outline", 0.0f, 0.0f, 0.0f);
    }

    // Model render
    m_model->processRecursive([&](Node& node) -> bool
        {
            if (&node == getSelected())
                return false;

            m_shader->setMat4("model", node.calcAbsoluteTransform());
            node.getMesh()->render(*m_renderSystem, *m_shader);

            return true;
        });

    // Origin render
    m_renderSystem->clearDepth();

    if (showOrigin)
    {
        m_origin->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget() * 0.15f)));
        m_origin->processRecursive([&](Node& node) -> bool
            {
                m_shader->setMat4("model", node.calcAbsoluteTransform());
                node.getMesh()->render(*m_renderSystem, *m_shader);

                return true;
            });
    }

    m_renderSystem->unbindFrame();

    // UI 
    m_guiSystem->begin();

    m_dockpaneLayer->render();
    m_propertiesLayer->render();
    m_treeLayer->render();
    m_consoleLayer->render();
    m_viewportLayer->render();
    m_settingsLayer->render();

    m_guiSystem->end();

    // Request delete
    if (m_deleted)
    {
        m_deleted->deleteFromParent();  // If child
        m_model->detachNode(m_deleted); // If root

        m_deleted = nullptr;
    }
}

void View::updateShadows()
{
    if (!castShadows)
    {
        m_renderSystem->bindDepth(m_depthId);

        m_renderSystem->setViewport(Settings::x, Settings::y, m_depthWidth, m_depthHeight);
        m_renderSystem->clearDepth();

        return;
    }

    float distance = m_viewport.getCamera().getDistanceToTarget() * 2.f;

    glm::mat4 lightProjection = glm::ortho(-distance, distance, -distance, distance, (float)m_viewport.getZNear(), (float)m_viewport.getZFar());
    glm::mat4 lightView = glm::lookAt(-lightDirection * distance, lightDirection, Settings::worldUp);

    m_lightSpaceMatrix = lightProjection * lightView;

    glCullFace(GL_FRONT);

    m_renderSystem->bindDepth(m_depthId);

    m_renderSystem->setViewport(Settings::x, Settings::y, m_depthWidth, m_depthHeight);
    m_renderSystem->clearDepth();

    m_shaderDepth->bind();
    m_shaderDepth->setMat4("lightSpaceMatrix", m_lightSpaceMatrix);

    m_model->processRecursive([&](Node& node) -> bool
        {
            m_shaderDepth->setMat4("model", node.calcAbsoluteTransform());
            node.getMesh()->render(*m_renderSystem, *m_shaderDepth);

            return true;
        });

    m_shaderDepth->unbind();

    m_renderSystem->unbindDepth();

    glCullFace(GL_BACK);
}

Model* View::getModel() const
{
    return m_model;
}

void View::setModel(Model* model)
{
    if (model)
    {
        m_model = model;
        zoomToFit();
    }
}

Node* View::getSelected() const
{
    return m_selected;
}

void View::setSelected(Node* selected)
{
    m_selected = selected;
}

Window& View::getWindow()
{
    return *m_window;
}

const Window& View::getWindow() const
{
    return *m_window;
}

Viewport& View::getViewport()
{
    return m_viewport;
}

const Viewport& View::getViewport() const
{
    return m_viewport;
}

ViewportLayer& View::getViewportLayer()
{
    return *m_viewportLayer;
}

const ViewportLayer& View::getViewportLayer() const
{
    return *m_viewportLayer;
}

void View::addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op)
{
    m_operatorDispatcher.addOperator(enterKey, exitKey, std::move(op));
}

void View::addOperator(ButtonCode button, std::unique_ptr<Operator> op)
{
    m_operatorDispatcher.addOperator(button, std::move(op));
}

void View::addOperator(KeyCode key, std::unique_ptr<Operator> op)
{
    m_operatorDispatcher.addOperator(key, std::move(op));
}

void View::zoomToFit()
{
    if (m_model)
    {
        if (!m_model->getNodes().empty())
        {
            Node* node = m_model->getNodes().front().get();

            if (node)
            {
                BoundaryBox start_bbox = node->getMesh()->getBoundingBox();
                const glm::mat4& start_mat = node->calcAbsoluteTransform();

                start_bbox.min = start_mat * glm::vec4(start_bbox.min, 1.0f);
                start_bbox.max = start_mat * glm::vec4(start_bbox.max, 1.0f);

                m_model->processRecursive([&](Node& node) -> bool
                    {
                        BoundaryBox bbox = node.getMesh()->getBoundingBox();
                        const glm::mat4& mat = node.calcAbsoluteTransform();

                        bbox.min = mat * glm::vec4(bbox.min, 1.0f);
                        bbox.max = mat * glm::vec4(bbox.max, 1.0f);

                        if (bbox.min.x < start_bbox.min.x)
                            start_bbox.min.x = bbox.min.x;
                        if (bbox.min.y < start_bbox.min.y)
                            start_bbox.min.y = bbox.min.y;
                        if (bbox.min.z < start_bbox.min.z)
                            start_bbox.min.z = bbox.min.z;

                        if (bbox.max.x > start_bbox.max.x)
                            start_bbox.max.x = bbox.max.x;
                        if (bbox.max.y > start_bbox.max.y)
                            start_bbox.max.y = bbox.max.y;
                        if (bbox.max.z > start_bbox.max.z)
                            start_bbox.max.z = bbox.max.z;

                        return true;
                    });


                double length = glm::length(start_bbox.max - start_bbox.min);

                m_viewport.setZFar(std::min(std::max(length, Settings::zfarMin), Settings::zfarMax));
                m_viewport.zoomToFit(start_bbox.min, start_bbox.max);
            }
        }
    }
}

void View::zoomToFit(Node* node)
{
    if (node)
    {
        BoundaryBox start_bbox = node->getMesh()->getBoundingBox();
        const glm::mat4& start_mat = node->calcAbsoluteTransform();

        start_bbox.min = start_mat * glm::vec4(start_bbox.min, 1.0f);
        start_bbox.max = start_mat * glm::vec4(start_bbox.max, 1.0f);

        node->processRecursive([&](Node& node) -> bool
            {
                BoundaryBox bbox = node.getMesh()->getBoundingBox();
                const glm::mat4& mat = node.calcAbsoluteTransform();

                bbox.min = mat * glm::vec4(bbox.min, 1.0f);
                bbox.max = mat * glm::vec4(bbox.max, 1.0f);

                if (bbox.min.x < start_bbox.min.x)
                    start_bbox.min.x = bbox.min.x;
                if (bbox.min.y < start_bbox.min.y)
                    start_bbox.min.y = bbox.min.y;
                if (bbox.min.z < start_bbox.min.z)
                    start_bbox.min.z = bbox.min.z;

                if (bbox.max.x > start_bbox.max.x)
                    start_bbox.max.x = bbox.max.x;
                if (bbox.max.y > start_bbox.max.y)
                    start_bbox.max.y = bbox.max.y;
                if (bbox.max.z > start_bbox.max.z)
                    start_bbox.max.z = bbox.max.z;

                return true;
            });

        m_viewport.zoomToFit(start_bbox.min, start_bbox.max);
    }
}

std::vector<Contact> View::raycast(double x, double y, FilterValue filterValues)
{
    std::vector<Node*> candidates;
    std::vector<Contact> contacts;
    Ray ray = m_viewport.calcCursorRay(x, y);

    // Broad Phase TODO Octree
    m_model->processRecursive([&](Node& node) -> bool
        {
            auto bbox = node.getMesh()->getBoundingBox();
            const auto& mat = node.calcAbsoluteTransform();

            bbox.min = mat * glm::vec4(bbox.min, 1.0f);
            bbox.max = mat * glm::vec4(bbox.max, 1.0f);

            if (glm::intersectAABB(ray.orig, ray.dir, bbox.min, bbox.max))
                candidates.push_back(&node);

            return true;
        });

    // Narrow Phase
    for (auto& node : candidates)
    {
        const auto& mat = node->calcAbsoluteTransform();
        const auto& table = node->getMesh()->getHalfEdgeTable();

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

void View::requestDelete(Node* node)
{
    m_deleted = node;
}

void View::decoratePlane(Node& plane) const
{
    std::unique_ptr<Mesh> mesh = Mesh::createPlane(Settings::worldUp, m_viewport.calcTargetPlaneWidth(), m_viewport.calcTargetPlaneWidth(), 16384);
    
    mesh->renderTriangles = false;
    mesh->renderLines = true;
    mesh->colorLines = Settings::colorGray;
    plane.attachMesh(std::move(mesh));
}

void View::decorateOrigin(Node& origin) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> arrowX = Mesh::createArrow(axisX, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowY = Mesh::createArrow(axisY, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowZ = Mesh::createArrow(axisZ, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    
    arrowX->setMaterial(Settings::red);
    arrowY->setMaterial(Settings::green);
    arrowZ->setMaterial(Settings::blue);
    
    origin.attachNode(std::make_unique<Node>());
    origin.attachNode(std::make_unique<Node>());
    origin.attachNode(std::make_unique<Node>());
    
    origin.getChildren()[0]->attachMesh(std::move(arrowX));
    origin.getChildren()[1]->attachMesh(std::move(arrowY));
    origin.getChildren()[2]->attachMesh(std::move(arrowZ));
}