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

class View
{
public:
    View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height);
    ~View();

    void update();

    Model* getModel() const;
    void setModel(Model* model);
    
    Node* getSelected() const;
    void setSelected(Node* selected);

    void addOperator(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Operator> op);
    void addOperator(ButtonCode button, std::unique_ptr<Operator> op);
    void addOperator(KeyCode key, std::unique_ptr<Operator> op);

    template<class Lambda>
    void addOperator(KeyCode key, Lambda lambda)
    {
        m_operatorDispatcher.addOperator(key, lambda);
    }

    void zoomToFit();
    std::vector<Contact> raycast(double x, double y, FilterValue filterValues);

    Viewport& getViewport();
    const Viewport& getViewport() const;

    Window* getWindow();
    const Window* getWindow() const;

    Node* getPlane();
    const Node* getPlane() const;

    Node* getOrigin();
    const Node* getOrigin() const;

    void decoratePlane(Node& plane) const;
    void decorateOrigin(Node& origin) const;
    void decorateTriad(Triad& triad) const;
    void decorateArrow(Manipulator& manipulator, glm::vec3 dir) const;

private:
    std::unique_ptr<Node> m_plane;
    std::unique_ptr<Node> m_origin;

    Model* m_model = nullptr;
    Node* m_selected = nullptr;
    
    Viewport m_viewport;

    Shader* m_shader = nullptr;
    RenderSystem* m_renderSystem = nullptr;
    
    uint32_t m_framebufferId;
    uint32_t m_framerenderId;
    uint32_t m_frametextureId;

    std::unique_ptr<Window> m_window;
    std::unique_ptr<GuiSystem> m_guiSystem;

    std::unique_ptr<ConsoleLayer> m_consoleLayer;
    std::unique_ptr<DockpaneLayer> m_dockpaneLayer;
    std::unique_ptr<PropertiesLayer> m_propertiesLayer;
    std::unique_ptr<TreeLayer> m_treeLayer;
    std::unique_ptr<ViewportLayer> m_viewportLayer;
    
    OperatorDispatcher m_operatorDispatcher;
};
