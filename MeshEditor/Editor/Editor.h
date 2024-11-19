#pragma once

#include <memory>
#include <string>
#include <vector>

#include "MeshEngine/Misc/DynamicLibrary.h"
#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Misc/Timer.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Parser/ColladaParser.h"
#include "MeshEngine/Parser/STLParser.h"
#include "MeshEngine/Parser/AssimpParser.h"

#include "MeshEngine/Scene/Scene.h"
#include "MeshEngine/Node/Node.h"

#include "Operator/View/Select.h"
#include "Operator/View/Pan.h"
#include "Operator/View/Arcball.h"

#include "Operator/Edit/EditNodeOperator.h"
#include "Operator/Edit/EditFaceOperator.h"
#include "Operator/Edit/EditVertexOperator.h"

#include "Operator/Delete/DeleteFaceOperator.h"

#include "View.h"

class Editor
{
public:
    View* createView(const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");

    std::unique_ptr<Node> loadModel(const std::string& filename);
    void saveModel(Node& model, const std::string& filename);

    uint32_t start();

    static Editor* instance();

private:
    Editor();
    ~Editor();
    Editor(Editor& other) = delete;
    void operator=(const Editor&) = delete;

    std::unique_ptr<RenderSystem> m_renderSystem;
    std::vector<std::unique_ptr<View>> m_views;

    STLParser m_stl;
    ColladaParser m_collada;
    AssimpParser m_assimp;
};

