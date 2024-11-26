#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "MeshEngine/RenderSystem/Window.h"
#include "MeshEngine/RenderSystem/GuiSystem.h"
#include "MeshEngine/RenderSystem/RenderSystem.h"

#include "MeshEngine/RootSystem/AssetSystem.h"
#include "MeshEngine/RootSystem/InputSystem.h"

#include "MeshEngine/Viewport/Viewport.h"
#include "MeshEngine/Math/Contact.h"

#include "MeshEngine/Scene/Scene.h"
#include "MeshEngine/Node/Node.h"

#include "MeshEditor/Node/OriginNode.h"
#include "MeshEditor/Node/PlaneNode.h"

#include "MeshEngine/Base/Base.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEditor/Operator/OperatorDispatcher.h"

#include "UI/ConsoleLayer.h"
#include "UI/DockpaneLayer.h"
#include "UI/PropertiesLayer.h"
#include "UI/TreeLayer.h"
#include "UI/ViewportLayer.h"
#include "UI/SettingsLayer.h"


class View : public Base
{
public:
    View(RenderSystem* renderSystem, const std::string& title, uint32_t width, uint32_t height, const std::string& icon = "");
    ~View();

public:
    void start();
    void end();

    void update(float deltaTime);
    void render();

public:
    Scene* getScene() const;
    void setScene(Scene* scene);
    
    Node* getSelected() const;
    void setSelected(Node* selected);

public:
    AssetSystem& getAssetSystem();
    InputSystem& getInputSystem();
    RenderSystem& getRenderSystem();
    GuiSystem& getGuiSystem();

public:
    Window& getWindow();
    Viewport& getViewport();

public:
    ViewportLayer& getViewportLayer();
    const ViewportLayer& getViewportLayer() const;

public:
    void addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op);
    void addOperator(ButtonCode button, std::unique_ptr<Operator> op);
    void addOperator(KeyCode key, std::unique_ptr<Operator> op);

    template<class Lambda>
    void addLambda(KeyCode key, Lambda lambda)
    {
        m_operatorDispatcher.addLambda(key, lambda);
    }

    void forceOperator(KeyCode key)
    {
        m_operatorDispatcher.forceOperator(*this, key);
    }

public:
    void zoomToFit();
    void zoomToFit(Node* node);

public:
    std::vector<Contact> raycast(double x, double y, FilterValue filterValues);

public:
    virtual void bind() override
    {
        bindProperty(showOrigin);
        bindProperty(showPlane);

        super::bind();
    }

    bool showOrigin = true;
    bool showPlane = true;

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<GuiSystem> m_guiSystem;
    
    std::unique_ptr<AssetSystem> m_assetSystem;
    std::unique_ptr<InputSystem> m_inputSystem;

    RenderSystem* m_renderSystem = nullptr;
    Shader* m_shaderSelected = nullptr;
    
    std::unique_ptr<ConsoleLayer> m_consoleLayer;
    std::unique_ptr<DockpaneLayer> m_dockpaneLayer;
    std::unique_ptr<PropertiesLayer> m_propertiesLayer;
    std::unique_ptr<TreeLayer> m_treeLayer;
    std::unique_ptr<ViewportLayer> m_viewportLayer;
    std::unique_ptr<SettingsLayer> m_settingsLayer;

    Scene* m_scene = nullptr;
    Node* m_selected = nullptr;

    std::unique_ptr<PlaneNode> m_plane;
    std::unique_ptr<OriginNode> m_origin;

    Viewport m_viewport;
    OperatorDispatcher m_operatorDispatcher;
};
