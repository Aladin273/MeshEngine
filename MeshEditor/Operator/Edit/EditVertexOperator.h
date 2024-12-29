#pragma once

#include "MeshEditor/Operator/Operator.h"
#include "MeshEditor/Editor/View.h"

class EditVertexOperator : public Operator
{
public:
    virtual void onEnter(View&) override;
    virtual void onExit(View&) override;
    virtual void onMouseMove(View& view, double x, double y) override;
    virtual void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
    virtual void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

    virtual void onUpdate(float deltaTime) override;
    virtual void onRender(RenderSystem* renderSystem) override;

private:
    void onSelectedChanged(View&, const Contact& selected);

private:
    bool m_active = false;
    
    Contact m_contact{};
    HalfEdgeVertexHandle m_vh{};
};