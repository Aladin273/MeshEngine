#include "Editor.h"

Editor* Editor::instance()
{
    static Editor Editor;
    return &Editor;
}

Editor::Editor()
{
    MeshEngine::Logger::init("Logs/MeshEditor.txt", 23, 55);
    m_renderSystem.reset(MeshEngine::createRenderSystem());
}

Editor::~Editor()
{
    m_renderSystem.release();
}

View* Editor::createView(const std::string& title, uint32_t width, uint32_t height, const std::string& icon)
{
    m_views.push_back(std::make_unique<View>(m_renderSystem.get(), title, width, height, icon));

    m_views.back()->addOperator(ButtonCode::Button_Middle, std::make_unique<PanOperator>());

    m_views.back()->addOperator(ButtonCode::Button_Middle, std::make_unique<ArcballOperator>());

    m_views.back()->addOperator(KeyCode::T, std::make_unique<EditNodeOperator>());

    m_views.back()->addOperator(KeyCode::E, std::make_unique<EditFaceOperator>());
    
    m_views.back()->addOperator(KeyCode::W, std::make_unique<EditVertexOperator>());
    
    m_views.back()->addOperator(KeyCode::D, KeyCode::Escape, std::make_unique<DeleteFaceOperator>());

    m_views.back()->addLambda(KeyCode::Q, [](View& view, Action action, Modifier mods)
    {
        exit(0);
    });

    m_views.back()->addLambda(KeyCode::F1, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setFrontView();
    });

    m_views.back()->addLambda(KeyCode::F2, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setTopView();
    });

    m_views.back()->addLambda(KeyCode::F3, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setRearView();
    });

    m_views.back()->addLambda(KeyCode::F4, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setRightView();
    });

    m_views.back()->addLambda(KeyCode::F5, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setLeftView();
    });

    m_views.back()->addLambda(KeyCode::F6, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setBottomView();
    });

    m_views.back()->addLambda(KeyCode::F7, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().getCamera().setIsoView();
    });

    m_views.back()->addLambda(KeyCode::F8, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.getViewport().setOrthogonal(!view.getViewport().getOrthogonal());
    });

    m_views.back()->addLambda(KeyCode::F9, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
            view.zoomToFit();
    });

    m_views.back()->addLambda(KeyCode::R, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getViewport().getCamera().setEyeTargetUp(MeshEngine::Settings::eye, MeshEngine::Settings::target, MeshEngine::Settings::up);
            view.zoomToFit();
        }
    });

    m_views.back()->addLambda(KeyCode::F, [](View& view, Action action, Modifier mods)
        {
            if (action == Action::Press)
            {
                view.zoomToFit(view.getSelected());
            }
        });

    m_views.back()->addLambda(KeyCode::S, [](View& view, Action action, Modifier mods)
    {
        //if (action == Action::Press && mods == Modifier::Control)
        //    Application::instance()->saveModel(*view.getModel(), view.getModel()->getName());
    });

    m_views.back()->addLambda(KeyCode::P, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.showPlane = !view.showPlane;
        }
    });

    m_views.back()->addLambda(KeyCode::O, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.showOrigin = !view.showOrigin;
        }
    });

    m_views.back()->addLambda(KeyCode::Delete, [](View& view, Action action, Modifier mods)
    {
        if (action == Action::Press)
        {
            view.getScene()->detachNode(view.getSelected());
            view.setSelected(nullptr);
        }
    });

    m_views.back()->forceOperator(KeyCode::T);

    return m_views.back().get();
}

uint32_t Editor::start()
{
    MeshEngine::Timer m_timer;

    for (auto& view : m_views)
        view->start();

    while (!m_views.empty())
    {
        float deltaTime = m_timer.elapsed();
        m_timer.reset();

        for (auto& view : m_views)
        {
            if (!MeshEngine::windowShouldClose(&view->getWindow()))
            {
                view->getWindow().setCurrentContext();
                
                view->update(deltaTime);
                view->render();

                MeshEngine::swapDisplayBuffers(&view->getWindow());
            }
            else
            {
                view->end();   
                view.reset();
            }
        }

        MeshEngine::pollEvents();
        m_views.erase(std::remove(m_views.begin(), m_views.end(), nullptr), m_views.end());
    }

    return 0;
}