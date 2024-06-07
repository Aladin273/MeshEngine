#pragma once

#include "BaseLayer.h"

#include <functional>

enum class ViewportMode : uint8_t
{
    Select = 0,
    Translate,
    Rotate,
    Scale,
    Universal,
    Bounds,
};

enum class GizmoMode : uint8_t
{
    World = 0,
    Local
};

class ViewportLayer : public BaseLayer
{
public:
    using GizmoCallback = std::function<void(const glm::mat4&)>;
    using FramebufferSizeCallback = std::function<void(double, double)>;

    ViewportLayer(View* view);

    virtual void render() override;

    void attach(uint32_t textureId);

    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;
    bool wantCaptureGizmo() const;

    void remapToRelative(double& x, double& y);

    void setViewportMode(ViewportMode mode);

    void setGizmoMode(GizmoMode mode);
    void setGizmoTransform(const glm::mat4& transform);

    void setGizmoCallback(const GizmoCallback& callback);
    void setFramebufferSizeCallback(const FramebufferSizeCallback& callback);

private:
    void guizmo();
    void overlay();

    uint32_t m_textureId;

    uint32_t m_width;
    uint32_t m_height;

    glm::vec2 m_mouse;
    glm::vec2 m_position;

    glm::vec2 m_min;
    glm::vec2 m_max;

    bool m_wantCaptureMouse = false;
    bool m_wantCaptureKeyboard = false;
    bool m_wantCaptureGizmo = false;

    ViewportMode m_viewportMode = ViewportMode::Select;
    GizmoMode m_gizmoMode = GizmoMode::World;

    glm::mat4 m_gizmoTransform{1.0f};

    GizmoCallback m_gizmoCallback;
    FramebufferSizeCallback m_sizeCallback;
};