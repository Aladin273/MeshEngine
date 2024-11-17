#pragma once

#include <memory>
#include <string>
#include <vector>

#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Misc/Timer.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Parsers/ColladaParser.h"
#include "MeshEngine/Parsers/STLParser.h"
#include "MeshEngine/Parsers/AssimpParser.h"

#include "MeshEngine/Model/Model.h"
#include "MeshEngine/Model/Scene.h"
#include "MeshEngine/Model/Node.h"
#include "MeshEngine/Model/MeshNode.h"

#include "Operators/View/Select.h"
#include "Operators/View/Pan.h"
#include "Operators/View/Arcball.h"

#include "Operators/Edit/EditNodeOperator.h"
#include "Operators/Edit/EditFaceOperator.h"
#include "Operators/Edit/EditVertexOperator.h"

#include "Operators/Delete/DeleteFaceOperator.h"

#include "View.h"

class Application
{
public:
    View* createView(const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");

    std::unique_ptr<Model> loadModel(const std::string& filename);
    void saveModel(const Model& model, const std::string& filename);

    void run();

    static Application* instance();

private:
    Application();
    ~Application();
    Application(Application& other) = delete;
    void operator=(const Application&) = delete;

    std::unique_ptr<RenderSystem> m_renderSystem;
    std::vector<std::unique_ptr<View>> m_views;

    STLParser m_stl;
    ColladaParser m_collada;
    AssimpParser m_assimp;
};

