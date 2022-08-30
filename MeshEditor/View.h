#pragma once

#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "OperatorDispatcher.h"
#include "Viewport.h"
#include "Model.h"

#include "Triad.h"
#include "Manipulator.h"

#include "Contact.h"
#include "FilterValue.h"

#include <IWindow.h>
#include <IGuiSystem.h>
#include <IRenderSystem.h>

#include "Utils.h"
#include "Settings.h"

class View
{
public:
    View(IRenderSystem* rs, const std::string& title, uint32_t width, uint32_t height);
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

    void decorateTriad(Triad& triad) const;
    void decorateArrow(Manipulator& manipulator, glm::vec3 dir) const;

    void zoomToFit();
    std::vector<Contact> raycast(double x, double y, FilterValue filterValues);

    Model* getModel() const;

    Viewport& getViewport();
    const Viewport& getViewport() const;

    IWindow* getWindow();
    const IWindow* getWindow() const;

    Node* getPlane();
    const Node* getPlane() const;

private:
    std::unique_ptr<Node> m_plane;

    Model* m_model = nullptr;
    Viewport m_viewport;

    IRenderSystem* m_renderSystem = nullptr;
    std::unique_ptr<IWindow> m_window;

    OperatorDispatcher m_operatorDispatcher;
};
