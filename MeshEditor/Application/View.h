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

class View
{
public:
    View(RenderSystem* rs, const std::string& title, uint32_t width, uint32_t height);
    ~View();

    void update();

    void setModel(Model* model);

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

    Model* getModel() const;

    Viewport& getViewport();
    const Viewport& getViewport() const;

    Window* getWindow();
    const Window* getWindow() const;

    Node* getPlane();
    const Node* getPlane() const;

    void decoratePlane(Node& plane) const;
    void decorateOrigin(Node& origin) const;
    void decorateTriad(Triad& triad) const;
    void decorateArrow(Manipulator& manipulator, glm::vec3 dir) const;

private:
    std::unique_ptr<Node> m_plane;
    std::unique_ptr<Node> m_origin;

    Model* m_model = nullptr;
    Viewport m_viewport;

    RenderSystem* m_renderSystem = nullptr;
    Shader* m_shader = nullptr;
    std::unique_ptr<Window> m_window;

    OperatorDispatcher m_operatorDispatcher;
};
