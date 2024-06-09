#include "Application.h"

Application* Application::instance()
{
    static Application instance;
    return &instance;
}

Application::Application()
{
    MeshEngine::Logger::init("Logs/MeshEditor.txt", 23, 55);

    m_renderSystem.reset(createRenderSystem());

    m_waitEvents = MeshEngine::waitEvents;
    m_pollEvents = MeshEngine::pollEvents;
    m_swapDisplayBuffers = MeshEngine::swapDisplayBuffers;
    m_windowShouldClose = MeshEngine::windowShouldClose;
}

Application::~Application()
{
    m_renderSystem.release();
}

View* Application::createView(const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    m_views.push_back(std::make_unique<View>(m_renderSystem.get(), title, width, height, icon));

    m_views.back()->addOperator(ButtonCode::Button_Left, std::make_unique<SelectOperator>());

    m_views.back()->addOperator(ButtonCode::Button_Middle, std::make_unique<PanOperator>());

    m_views.back()->addOperator(ButtonCode::Button_Middle, std::make_unique<ArcballOperator>());

    m_views.back()->addOperator(KeyCode::D, KeyCode::Escape, std::make_unique<DeleteFaceOperator>());
    
    m_views.back()->addOperator(KeyCode::E, KeyCode::Escape, std::make_unique<EditFaceOperator>());
    
    m_views.back()->addOperator(KeyCode::W, KeyCode::Escape, std::make_unique<EditVertexOperator>());

    m_views.back()->addOperator(KeyCode::T, KeyCode::Escape, std::make_unique<EditNodeOperator>());

    m_views.back()->addOperator(KeyCode::Q, [](View& view, Action action, Modifier mods)
    {
        exit(0);
    });

    m_views.back()->addOperator(KeyCode::F1, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setFrontView();
    });

    m_views.back()->addOperator(KeyCode::F2, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setTopView();
    });

    m_views.back()->addOperator(KeyCode::F3, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setRearView();
    });

    m_views.back()->addOperator(KeyCode::F4, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setRightView();
    });

    m_views.back()->addOperator(KeyCode::F5, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setLeftView();
    });

    m_views.back()->addOperator(KeyCode::F6, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setBottomView();
    });

    m_views.back()->addOperator(KeyCode::F7, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setIsoView();
    });

    m_views.back()->addOperator(KeyCode::F8, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().setOrthogonal(!view.getViewport().getOrthogonal());
    });

    m_views.back()->addOperator(KeyCode::F9, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.zoomToFit();
    });

    m_views.back()->addOperator(KeyCode::R, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getViewport().getCamera().setEyeTargetUp(Settings::eye, Settings::target, Settings::up);
            view.zoomToFit();
        }
    });

    m_views.back()->addOperator(KeyCode::F, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                view.zoomToFit(view.getSelected());
            }
        });

    m_views.back()->addOperator(KeyCode::S, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press && mods == Modifier::Control)
            Application::instance()->saveModel(*view.getModel(), view.getModel()->getName());
    });

    m_views.back()->addOperator(KeyCode::H, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getModel()->processRecursive([](Node& node)
                {
                    node.getMesh()->renderHoles = !node.getMesh()->renderHoles;
                    return true;
                });
        }
    });

    m_views.back()->addOperator(KeyCode::J, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getModel()->processRecursive([](Node& node)
                {
                    node.getMesh()->renderLines = !node.getMesh()->renderLines;
                    return true;
                });
        }
    });

    m_views.back()->addOperator(KeyCode::K, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getModel()->processRecursive([](Node& node)
                {
                    node.getMesh()->renderTriangles = !node.getMesh()->renderTriangles;
                    return true;
                });
        }
    });

    m_views.back()->addOperator(KeyCode::L, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getModel()->processRecursive([](Node& node)
                {
                    node.getMesh()->renderBoundaries = !node.getMesh()->renderBoundaries;
                    return true;
                });
        }
    });

    m_views.back()->addOperator(KeyCode::P, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.showPlane = !view.showPlane;
        }
    });

    m_views.back()->addOperator(KeyCode::O, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.showOrigin = !view.showOrigin;
        }
    });

    m_views.back()->addOperator(KeyCode::Delete, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.requestDelete(view.getSelected());
            view.setSelected(nullptr);
        }
    });

    return m_views.back().get();
}

std::unique_ptr<Model> Application::loadModel(const std::string& filename)
{
    if (filename.find(".stl") != Settings::invalid)
        return m_stl.loadModel(filename);
    else if (filename.find(".dae") != Settings::invalid)
        return m_collada.loadModel(filename);
    else
        return m_assimp.loadModel(filename);
}

void Application::saveModel(const Model& model, const std::string& filename)
{
    if (filename.find(".stl") != Settings::invalid)
        m_stl.saveModel(model, filename);
    else
        m_collada.saveModel(model, filename);
}
void Application::run()
{
    while (!m_views.empty())
    {
        for (auto& view : m_views)
        {
            if (!m_windowShouldClose(&view->getWindow()))
            {
                view->getWindow().setCurrentContext();
                view->update();

                m_swapDisplayBuffers(&view->getWindow());
            }
            else
                view.reset();
        }

        m_pollEvents();
        m_views.erase(std::remove(m_views.begin(), m_views.end(), nullptr), m_views.end());
    }
}

Window* Application::createWindow(const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{;
    return MeshEngine::createWindow(title, width, height, icon);
}

GuiSystem* Application::createGuiSystem(Window* window)
{;
    return MeshEngine::createGuiSystem(window);
}

RenderSystem* Application::createRenderSystem()
{;
    return MeshEngine::createRenderSystem();
}

Shader* Application::createShader(const std::string& vertexPath, const std::string& fragmentPath, const std::string& geometryPath)
{
    return MeshEngine::createShader(vertexPath, fragmentPath, geometryPath);
}
