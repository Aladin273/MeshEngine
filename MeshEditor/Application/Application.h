#pragma once

#include <memory>
#include <string>
#include <vector>

#include "MeshEngine/RenderSystem/Export.h"
#include "MeshEngine/Parsers/ColladaParser.h"
#include "MeshEngine/Parsers/STLParser.h"
#include "MeshEngine/Model/Model.h"

#include "Operators/View/Pan.h"
#include "Operators/View/Arcball.h"
#include "Operators/Edit/EditNodeOperator.h"
#include "Operators/Edit/EditFaceOperator.h"
#include "Operators/Edit/EditVertexOperator.h"
#include "Operators/Delete/DeleteFaceOperator.h"

#include "Settings.h"
#include "View.h"

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

    STLParser m_stl;
    ColladaParser m_collada;

    std::function<void()> m_waitEvents;
    std::function<void()> m_pollEvents;
    std::function<void(IWindow*)> m_swapDisplayBuffers;
    std::function<bool(IWindow*)> m_windowShouldClose;
};

