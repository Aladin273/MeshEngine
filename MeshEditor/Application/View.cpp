#include "View.h"

View::View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    m_window.reset(MeshEngine::createWindow(title, width, height, icon));
    m_guiSystem.reset(MeshEngine::createGuiSystem(m_window.get()));

    m_guiSystem->init();

    m_renderSystem = rs;
    m_renderSystem->init();

    m_shaderEditor.reset(MeshEngine::createShader(MeshEngine::Settings::shadersPath + "editorVertex.glsl", MeshEngine::Settings::shadersPath + "editorFragment.glsl"));
    m_shaderOutline.reset(MeshEngine::createShader(MeshEngine::Settings::shadersPath + "outlineVertex.glsl", MeshEngine::Settings::shadersPath + "outlineFragment.glsl"));

    m_dockpaneLayer = std::make_unique<DockpaneLayer>(this);
    m_consoleLayer = std::make_unique<ConsoleLayer>(this);
    m_propertiesLayer = std::make_unique<PropertiesLayer>(this);
    m_treeLayer = std::make_unique<TreeLayer>(this);
    m_settingsLayer = std::make_unique<SettingsLayer>(this);
    m_viewportLayer = std::make_unique<ViewportLayer>(this);

    m_renderSystem->bufferFrame(m_viewportLayer->frameId, m_viewportLayer->frameRenderId, m_viewportLayer->frameTextureId, width, height);

    m_viewport.getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
    m_viewport.setViewportSize(width, height);
    m_viewport.setFOV(MeshEngine::Settings::fov);
    m_viewport.setZNear(MeshEngine::Settings::znear);
    m_viewport.setZFar(MeshEngine::Settings::zfar);

    m_plane = std::make_unique<PlaneNode>(MeshEngine::Settings::worldUp, m_viewport.calcTargetPlaneWidth(), m_viewport.calcTargetPlaneHeight(), 16384);
    m_origin = std::make_unique<OriginNode>();

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
                double factor = y > MeshEngine::Settings::invalid ? MeshEngine::Settings::zoomIn : MeshEngine::Settings::zoomOut;
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

            m_renderSystem->unbufferFrame(m_viewportLayer->frameId);
            m_renderSystem->bufferFrame(m_viewportLayer->frameId, m_viewportLayer->frameRenderId, m_viewportLayer->frameTextureId, width, height);
        });
}

View::~View()
{
    m_window.reset();
}

void View::update(float deltaTime)
{
    // Scene
    //////////////////////////////////////////////////
    m_scene->update(deltaTime);

    // Editor
    //////////////////////////////////////////////////
    m_renderSystem->bindFrame(m_viewportLayer->frameId);
    m_renderSystem->setViewport(MeshEngine::Settings::x, MeshEngine::Settings::y, m_viewport.getWidth(), m_viewport.getHeight());

    // Selected Render
    //////////////////////////////////////////////////
    if (getSelected())
    {
        m_shaderOutline->bind();

        m_shaderOutline->setMat4("view", m_viewport.getCamera().calcViewMatrix());
        m_shaderOutline->setMat4("projection", m_viewport.calcProjectionMatrix());
    
        getSelected()->processRecursive([&](Node& node) -> bool
            {
                if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
                {
                    m_shaderOutline->setMat4("model", meshNode->getAbsoluteTransform());
                    meshNode->getMesh()->render(*m_renderSystem, *m_shaderOutline);
                }
    
                return true;
            });

        m_shaderOutline->unbind();
    }

    // Editor Shader
    //////////////////////////////////////////////////
    m_shaderEditor->bind();

    m_shaderEditor->setMat4("view", m_viewport.getCamera().calcViewMatrix());
    m_shaderEditor->setMat4("projection", m_viewport.calcProjectionMatrix());

    // Plane render
    //////////////////////////////////////////////////
    if (showPlane)
    {
        m_plane->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget())));
        m_plane->processRecursive([&](Node& node) -> bool
            {
                if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
                {
                    m_shaderEditor->setMat4("model", meshNode->getAbsoluteTransform());
                    meshNode->getMesh()->render(*m_renderSystem, *m_shaderEditor);
                }

                return true;
            });

    }

    // Origin render
    //////////////////////////////////////////////////
    if (showOrigin)
    {
        m_renderSystem->clearDepth();

        m_origin->setRelativeTransform(glm::scale(glm::vec3(m_viewport.getCamera().getDistanceToTarget() * 0.15f)));
        m_origin->processRecursive([&](Node& node) -> bool
            {
                if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
                {
                    m_shaderEditor->setMat4("model", meshNode->getAbsoluteTransform());
                    meshNode->getMesh()->render(*m_renderSystem, *m_shaderEditor);
                }

                return true;
            });
    }

    m_shaderEditor->unbind();
    m_renderSystem->unbindFrame();

    // UI 
    //////////////////////////////////////////////////
    m_guiSystem->begin();

    m_dockpaneLayer->render();
    m_propertiesLayer->render();
    m_treeLayer->render();
    m_consoleLayer->render();
    m_viewportLayer->render();
    m_settingsLayer->render();

    m_guiSystem->end();
}

Scene* View::getScene() const
{
    return m_scene;
}

void View::setScene(Scene* scene)
{
    if (scene)
    {
        m_scene = scene;
        m_scene->init(m_renderSystem);
        
        m_scene->setRenderTarget(m_viewportLayer->frameId);
        m_scene->setViewport(m_viewport);

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
    BoundingBox bbox = m_scene->getBoundingBox();
    double length = glm::distance(bbox.min, bbox.max);

    m_viewport.setZFar(std::clamp(length, MeshEngine::Settings::zfarMin, MeshEngine::Settings::zfarMax));
    m_viewport.zoomToFit(bbox.min, bbox.max);
}

void View::zoomToFit(Node* node)
{
    if (node)
    {
        BoundingBox bbox = node->getBoundingBox();
        bbox.tranform(node->getAbsoluteTransform());

        m_viewport.zoomToFit(bbox.min, bbox.max);
    }
}

std::vector<Contact> View::raycast(double x, double y, FilterValue filterValues)
{
    Ray ray = m_viewport.calcCursorRay(x, y);
    return m_scene->raycast(ray, filterValues);
}
