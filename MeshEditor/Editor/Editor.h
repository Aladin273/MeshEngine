#pragma once

#include <memory>
#include <string>
#include <vector>

#include "MeshEngine/Misc/DynamicLibrary.h"
#include "MeshEngine/Misc/Export.h"
#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Misc/Timer.h"
#include "MeshEngine/Misc/Settings.h"

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

public:
    uint32_t start(); 
    static Editor* instance();

private:
    Editor();
    ~Editor();
    Editor(Editor& other) = delete;
    void operator=(const Editor&) = delete;

    std::unique_ptr<RenderSystem> m_renderSystem;
    std::vector<std::unique_ptr<View>> m_views;
};

