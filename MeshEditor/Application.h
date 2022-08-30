#pragma once

#include <memory>
#include <string>
#include <vector>

#include "View.h"
#include "Model.h"

#include "DynamicLibrary.h"
#include "ColladaParser.h"
#include "STLParser.h"
#include "Settings.h"

#include "Pan.h"
#include "Arcball.h"
#include "DeleteFace.h"
#include "EditMeshOperator.h"
#include "EditVertexOperator.h"
#include "TransformNodeOperator.h"

class Application
{
public:
    View* createView(const std::string& title, uint32_t width, uint32_t height);

    std::unique_ptr<Model> loadModel(const std::string& filename);
    void saveModel(const Model& model, const std::string& filename);

    void run();

    IWindow* createWindow(const std::string&, uint32_t, uint32_t);
    IGuiSystem* createGuiSystem(IWindow*);
    IRenderSystem* createRenderSystem();

    static Application* instance();

private:
    Application();
    ~Application();
    Application(Application& other) = delete;
    void operator=(const Application&) = delete;

    std::unique_ptr<IRenderSystem> m_renderSystem;
    std::vector<std::unique_ptr<View>> m_views;

    ColladaParser m_collada;
    STLParser m_stl;

    std::unique_ptr<DynamicLibrary> m_dll;
    std::function<void()> m_waitEvents;
    std::function<void()> m_pollEvents;
    std::function<void(IWindow*)> m_swapDisplayBuffers;
    std::function<bool(IWindow*)> m_windowShouldClose;
};

