#include "View.h"
#include "Settings.h"
#include "Application.h"

View::View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height)
{
    m_window.reset(Application::instance()->createWindow(title, width, height));
    m_guiSystem.reset(Application::instance()->createGuiSystem(m_window.get()));

    m_guiSystem->init();

    m_renderSystem = rs;
    m_renderSystem->init();
    m_renderSystem->setViewport(Settings::x, Settings::y, width, height);
    m_renderSystem->bufferFrame(m_framebufferId, m_framerenderId, m_frametextureId, width, height);
    
    m_shader = Application::instance()->createShader(Settings::shadersPath + "vertex.glsl", Settings::shadersPath + "fragment.glsl");

    m_dockpaneLayer = std::make_unique<DockpaneLayer>(this);
    m_consoleLayer = std::make_unique<ConsoleLayer>(this);
    m_propertiesLayer = std::make_unique<PropertiesLayer>(this);
    m_treeLayer = std::make_unique<TreeLayer>(this);
    
    m_viewportLayer = std::make_unique<ViewportLayer>(this);
    m_viewportLayer->attach(m_frametextureId);

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
                if (y > Settings::invalid)
                    m_viewport.getCamera().zoom(Settings::zoomIn);
                else
                    m_viewport.getCamera().zoom(Settings::zoomOut);
            }
        });

    m_window->setFramebufferSizeCallback([&](int width, int height)
        {
            //m_viewport.setViewportSize(width, height);
            //
            //m_renderSystem->unbufferFrame(m_framebufferId);
            //m_renderSystem->bufferFrame(m_framebufferId, m_framerenderId, m_frametextureId, width, height);
        });

    m_viewportLayer->setFramebufferSizeCallback([&](int width, int height)
        {
            m_viewport.setViewportSize(width, height);

            m_renderSystem->unbufferFrame(m_framebufferId);
            m_renderSystem->bufferFrame(m_framebufferId, m_framerenderId, m_frametextureId, width, height);

            m_viewportLayer->attach(m_frametextureId);
        });
}

View::~View()
{
    m_window.reset();
}

void View::update()
{
    m_renderSystem->bindFrame(m_framebufferId);

    std::vector<Node*> postRender;

    m_renderSystem->setViewport(Settings::x, Settings::y, m_viewport.getWidth(), m_viewport.getHeight());
    m_renderSystem->clearDisplay(Settings::colorBackground.r, Settings::colorBackground.g, Settings::colorBackground.b, Settings::colorBackground.a);
    
    m_shader->setMat4("view", m_viewport.getCamera().calcViewMatrix());
    m_shader->setMat4("projection", m_viewport.calcProjectionMatrix());

    m_shader->setInt("numDirLights", 1);
    m_shader->setInt("numPointLights", 0);
    m_shader->setInt("numSpotLights", 0);

    m_shader->setVec3("dirLights[0].direction", m_viewport.getCamera().calcForward());
    m_shader->setVec3("dirLights[0].ambient", Settings::ambient);
    m_shader->setVec3("dirLights[0].diffuse", Settings::diffuse);
    m_shader->setVec3("dirLights[0].specular", Settings::specular);

    // Plane render
    m_plane->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget())));
    m_shader->setMat4("model", m_plane->calcAbsoluteTransform());
    m_plane->getMesh()->render(*m_renderSystem, *m_shader);

    // Model render
    m_model->processRecursive([&](Node& node)
        {
            if (dynamic_cast<Manipulator*>(&node) == nullptr)
            {
                if (&node == getSelected())
                    m_shader->setVec3("outline", 0.2f, 0.2f, 0.2f);

                m_shader->setMat4("model", node.calcAbsoluteTransform());
                node.getMesh()->render(*m_renderSystem, *m_shader);

                m_shader->setVec3("outline", 0.0f, 0.0f, 0.0f);
            }
            else
                postRender.push_back(&node);
        });

    m_renderSystem->clearDepth();

    // Origin render
    m_origin->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget() * 0.15f)));
    m_origin->processRecursive([&](Node& node)
        {
            m_shader->setMat4("model", node.calcAbsoluteTransform());
            node.getMesh()->render(*m_renderSystem, *m_shader);
        });

    // Post Render
    for (auto& node : postRender)
    {
        m_shader->setMat4("model", node->calcAbsoluteTransform());
        node->getMesh()->render(*m_renderSystem, *m_shader);
    }

    m_renderSystem->unbindFrame();

    // UI 
    m_guiSystem->begin();

    m_dockpaneLayer->render();
    m_propertiesLayer->render();
    m_treeLayer->render();
    m_consoleLayer->render();
    m_viewportLayer->render();

    m_guiSystem->end();
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

                m_model->processRecursive([&](Node& node)
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
                    });

                m_viewport.zoomToFit(start_bbox.min, start_bbox.max);
                m_viewport.setZFar(glm::length(start_bbox.max - start_bbox.min * 100.f));
            }
        }
    }
}

std::vector<Contact> View::raycast(double x, double y, FilterValue filterValues)
{
    std::vector<Node*> candidates;
    std::vector<Contact> contacts;
    Ray ray = m_viewport.calcCursorRay(x, y);

    // Broad Phase TODO Octree
    m_model->processRecursive([&](Node& node)
        {
            auto bbox = node.getMesh()->getBoundingBox();
            const auto& mat = node.calcAbsoluteTransform();

            bbox.min = mat * glm::vec4(bbox.min, 1.0f);
            bbox.max = mat * glm::vec4(bbox.max, 1.0f);

            if (glm::intersectAABB(ray.orig, ray.dir, bbox.min, bbox.max))
                candidates.push_back(&node);
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
            glm::vec3 a = mat * glm::vec4(table.getEndPoint(heh0), 1.0f);
            glm::vec3 b = mat * glm::vec4(table.getEndPoint(heh1), 1.0f);
            glm::vec3 c = mat * glm::vec4(table.getEndPoint(heh2), 1.0f);

            if (glm::intersectRayTriangle(ray.orig, ray.dir, t, a, b, c))
                contacts.push_back({ table.handle(face), node, t, ray.orig + ray.dir * t });

            if (heh3 != heh0)
            {
                glm::vec3 d = mat * glm::vec4(table.getEndPoint(heh3), 1.0f);

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

Viewport& View::getViewport()
{
    return m_viewport;
}

const Viewport& View::getViewport() const
{
    return m_viewport;
}

Window* View::getWindow()
{
    return m_window.get();
}

const Window* View::getWindow() const
{
    return m_window.get();
}

Node* View::getPlane()
{
    return m_plane.get();
}

const Node* View::getPlane() const
{
    return m_plane.get();
}

Node* View::getOrigin()
{
    return m_origin.get();
}

const Node* View::getOrigin() const
{
    return m_origin.get();
}

void View::decoratePlane(Node& plane) const
{
    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(heds::createPlane(
        Settings::worldUp, m_viewport.calcTargetPlaneWidth(), m_viewport.calcTargetPlaneWidth(), 16384));

    mesh->colorLines = Settings::colorGray;
    mesh->renderTriangles = false;

    plane.attachMesh(std::move(mesh));
}

void View::decorateOrigin(Node& origin) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> arrowX = std::make_unique<Mesh>(heds::createArrow(axisX, pointTR, pointTL, shaftTR, shaftTL, numSubs));
    std::unique_ptr<Mesh> arrowY = std::make_unique<Mesh>(heds::createArrow(axisY, pointTR, pointTL, shaftTR, shaftTL, numSubs));
    std::unique_ptr<Mesh> arrowZ = std::make_unique<Mesh>(heds::createArrow(axisZ, pointTR, pointTL, shaftTR, shaftTL, numSubs));

    arrowX->renderTriangles = false;
    arrowY->renderTriangles = false;
    arrowZ->renderTriangles = false;

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

void View::decorateTriad(Triad& triad) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> arrowX = std::make_unique<Mesh>(heds::createArrow(axisX, pointTR, pointTL, shaftTR, shaftTL, numSubs));
    std::unique_ptr<Mesh> arrowY = std::make_unique<Mesh>(heds::createArrow(axisY, pointTR, pointTL, shaftTR, shaftTL, numSubs));
    std::unique_ptr<Mesh> arrowZ = std::make_unique<Mesh>(heds::createArrow(axisZ, pointTR, pointTL, shaftTR, shaftTL, numSubs));

    std::unique_ptr<Mesh> torusX = std::make_unique<Mesh>(heds::createTorus(axisX, minorTR, majorTR, numSubs));
    std::unique_ptr<Mesh> torusY = std::make_unique<Mesh>(heds::createTorus(axisY, minorTR, majorTR, numSubs));
    std::unique_ptr<Mesh> torusZ = std::make_unique<Mesh>(heds::createTorus(axisZ, minorTR, majorTR, numSubs));

    std::unique_ptr<Mesh> cubeX = std::make_unique<Mesh>(heds::createCube(axisX * cubeTL, cubeTR));
    std::unique_ptr<Mesh> cubeY = std::make_unique<Mesh>(heds::createCube(axisY * cubeTL, cubeTR));
    std::unique_ptr<Mesh> cubeZ = std::make_unique<Mesh>(heds::createCube(axisZ * cubeTL, cubeTR));

    arrowX->setMaterial(Settings::red);
    arrowY->setMaterial(Settings::green);
    arrowZ->setMaterial(Settings::blue);

    torusX->setMaterial(Settings::red);
    torusY->setMaterial(Settings::green);
    torusZ->setMaterial(Settings::blue);

    cubeX->setMaterial(Settings::red);
    cubeY->setMaterial(Settings::green);
    cubeZ->setMaterial(Settings::blue);

    triad.getChildren()[0]->attachMesh(std::move(arrowX));
    triad.getChildren()[1]->attachMesh(std::move(arrowY));
    triad.getChildren()[2]->attachMesh(std::move(arrowZ));
         
    triad.getChildren()[3]->attachMesh(std::move(torusX));
    triad.getChildren()[4]->attachMesh(std::move(torusY));
    triad.getChildren()[5]->attachMesh(std::move(torusZ));
         
    triad.getChildren()[6]->attachMesh(std::move(cubeX));
    triad.getChildren()[7]->attachMesh(std::move(cubeY));
    triad.getChildren()[8]->attachMesh(std::move(cubeZ));

    for (auto& child : triad.getChildren())
    {
        glm::vec3 center = child->getRelativeTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        child->applyRelativeTransform(glm::translate(-center));
        child->applyRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget() * Settings::sizeT)));
        child->applyRelativeTransform(glm::translate(center));
    }
}

void View::decorateArrow(Manipulator& manipulator, glm::vec3 dir) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(heds::createArrow(dir, pointAR, pointAL, shaftAR, shaftAL, numSubs));

    mesh->setMaterial(Settings::blue);
    manipulator.attachMesh(std::move(mesh));
    
    glm::vec3 center = manipulator.getRelativeTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    manipulator.applyRelativeTransform(glm::translate(-center));
    manipulator.applyRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget() * Settings::sizeA)));
    manipulator.applyRelativeTransform(glm::translate(center));
}
