#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "MeshEngine/RenderSystem/Window.h"
#include "MeshEngine/RenderSystem/GuiSystem.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"

#include "MeshEngine/Viewport/Viewport.h"
#include "MeshEngine/Model/Model.h"
#include "MeshEngine/Manipulators/Triad.h"
#include "MeshEngine/Math/Intersections.h"

#include "MeshEditor/Operators/Contact.h"
#include "MeshEditor/Operators/OperatorDispatcher.h"

#include "UI/ConsoleLayer.h"
#include "UI/DockpaneLayer.h"
#include "UI/PropertiesLayer.h"
#include "UI/TreeLayer.h"
#include "UI/ViewportLayer.h"
#include "UI/SettingsLayer.h"

#include "Settings.h"

class View
{
public:
    View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");
    ~View();

    bool cameraLight = false;
    bool flatShading = true;
    bool castShadows = true;
    bool showOrigin = false;
    bool showPlane = false;

    bool selectBlocked = false;

    glm::vec3 lightDirection{ -0.5f, -0.75f, -1.0f };
    glm::vec4 backgroundColor{ Settings::colorBackground };

public:
    void update();

    void updateModel();
    
    void updateShadows();

    Model* getModel() const;
    void setModel(Model* model);
    
    Node* getSelected() const;
    void setSelected(Node* selected);

    Window* getWindow();
    const Window* getWindow() const;

    Viewport& getViewport();
    const Viewport& getViewport() const;

    void addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op);
    void addOperator(ButtonCode button, std::unique_ptr<Operator> op);
    void addOperator(KeyCode key, std::unique_ptr<Operator> op);

    template<class Lambda>
    void addOperator(KeyCode key, Lambda lambda)
    {
        m_operatorDispatcher.addOperator(key, lambda);
    }

    void zoomToFit();
    void zoomToFit(Node* node);

    std::vector<Contact> raycast(double x, double y, FilterValue filterValues);

private:
    void decoratePlane(Node& plane) const;
    void decorateOrigin(Node& origin) const;

private:
    std::unique_ptr<Node> m_plane;
    std::unique_ptr<Node> m_origin;

    Model* m_model = nullptr;
    Node* m_selected = nullptr;
    
    Viewport m_viewport;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<GuiSystem> m_guiSystem;

    RenderSystem* m_renderSystem = nullptr;
    
    Shader* m_shader = nullptr;
    Shader* m_shaderDepth = nullptr;
    
    // Viewport
    uint32_t m_frameId = 0;
    uint32_t m_frameRenderId = 0;
    uint32_t m_frameTextureId = 0;

    // Shadows
    uint32_t m_depthId = 0;
    uint32_t m_depthTextureId = 0;
    uint32_t m_depthWidth = 1024;
    uint32_t m_depthHeight = 1024;
    glm::mat4 m_lightSpaceMatrix{1.0f};

    // UI
    std::unique_ptr<ConsoleLayer> m_consoleLayer;
    std::unique_ptr<DockpaneLayer> m_dockpaneLayer;
    std::unique_ptr<PropertiesLayer> m_propertiesLayer;
    std::unique_ptr<TreeLayer> m_treeLayer;
    std::unique_ptr<ViewportLayer> m_viewportLayer;
    std::unique_ptr<SettingsLayer> m_settingsLayer;
    
    OperatorDispatcher m_operatorDispatcher;
};
