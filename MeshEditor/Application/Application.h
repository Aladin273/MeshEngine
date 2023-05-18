#pragma once

#include <memory>
#include <string>
#include <vector>

#include "MeshEngine/Misc/DynamicLibrary.h"
#include "MeshEngine/Parsers/ColladaParser.h"
#include "MeshEngine/Parsers/STLParser.h"
#include "MeshEngine/Model/Model.h"

#include "MeshEditor/Operators/View/Pan.h"
#include "MeshEditor/Operators/View/Arcball.h"
#include "MeshEditor/Operators/Edit/EditNodeOperator.h"
#include "MeshEditor/Operators/Edit/EditFaceOperator.h"
#include "MeshEditor/Operators/Edit/EditVertexOperator.h"
#include "MeshEditor/Operators/Delete/DeleteFaceOperator.h"

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

    std::unique_ptr<DynamicLibrary> m_dll;
    std::function<void()> m_waitEvents;
    std::function<void()> m_pollEvents;
    std::function<void(IWindow*)> m_swapDisplayBuffers;
    std::function<bool(IWindow*)> m_windowShouldClose;
};

