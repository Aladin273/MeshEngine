#include "Application.h"

Application* Application::instance()
{
    static Application instance;
    return &instance;
}

Application::Application()
{
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

View* Application::createView(const std::string& title, uint32_t width, uint32_t height)
{
    m_views.push_back(std::make_unique<View>(m_renderSystem.get(), title, width, height));

    m_views.back()->addOperator(ButtonCode::Button_Left, std::make_unique<PanOperator>());

    m_views.back()->addOperator(ButtonCode::Button_Right, std::make_unique<ArcballOperator>());

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
                static int i = Settings::invalid; ++i;

                view.getModel()->processRecursive([](Node& node)
                    {
                        switch (i)
                        {
                        case 0: node.getMesh()->setMaterial(Settings::ruby); break;
                        case 1: node.getMesh()->setMaterial(Settings::emerald); break;
                        case 2: node.getMesh()->setMaterial(Settings::gold); break;
                        case 3: node.getMesh()->setMaterial(Settings::copper); break;
                        case 4: node.getMesh()->setMaterial(Settings::chrome); break;
                        case 5: node.getMesh()->setMaterial(Settings::dark); break;
                        case 6: node.getMesh()->setMaterial(Settings::clear); break;
                        }
                    });;

                if (i == 6)
                    i = Settings::invalid;
            }
        });

    m_views.back()->addOperator(KeyCode::G, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                view.getModel()->processRecursive([](Node& node)
                    {
                        if (node.getMesh()->colorLines == Settings::colorWhite)
                        {
                            node.getMesh()->colorLines = Settings::colorBlack;
                        }
                        else
                        {
                            node.getMesh()->colorLines = Settings::colorWhite;
                        }
                    });;
            }
        });

    m_views.back()->addOperator(KeyCode::V, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                View* newView = Application::instance()->createView(Settings::title, Settings::width, Settings::height);
                newView->setModel(view.getModel());
            }
        });

    m_views.back()->addOperator(KeyCode::S, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
                Application::instance()->saveModel(*view.getModel(), view.getModel()->getName());
        });

    m_views.back()->addOperator(KeyCode::H, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                view.getModel()->processRecursive([](Node& node)
                    {
                        node.getMesh()->renderHoles = !node.getMesh()->renderHoles;
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
                    });
            }
        });

    m_views.back()->addOperator(KeyCode::P, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                static size_t state = 0;
                Node* plane = view.getPlane();

                if (state == 0)
                {
                    std::unique_ptr<Mesh> mesh = std::make_unique<Mesh>(heds::createPlane(Settings::world_up,
                        view.getViewport().calcTargetPlaneWidth(), view.getViewport().calcTargetPlaneWidth(), Settings::plane));
                    
                    plane->attachMesh(std::move(mesh));
                    plane->getMesh()->colorLines = Settings::colorWhite;
                    plane->getMesh()->renderTriangles = false;
                    plane->getMesh()->renderLines = true;
                    ++state;
                }
                else if (state == 1)
                {
                    plane->getMesh()->colorLines = Settings::colorBlack;
                    ++state;
                }
                else
                {
                    plane->getMesh()->renderLines = false;
                    state = 0;
                }
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
        return nullptr;
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
            if (!m_windowShouldClose(view->getWindow()))
            {
                view->getWindow()->setCurrentContext();
                view->update();

                m_swapDisplayBuffers(view->getWindow());
            }
            else
                view.reset();
        }

        m_pollEvents();
        m_views.erase(std::remove(m_views.begin(), m_views.end(), nullptr), m_views.end());
    }
}

IWindow* Application::createWindow(const std::string& title, uint32_t width, uint32_t height)
{;
    return MeshEngine::createWindow(title, width, height);
}

IGuiSystem* Application::createGuiSystem(IWindow* window)
{;
    return MeshEngine::createGuiSystem(window);
}

IRenderSystem* Application::createRenderSystem()
{;
    return MeshEngine::createRenderSystem();
}
