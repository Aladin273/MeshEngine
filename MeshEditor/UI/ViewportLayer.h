#pragma once

#include "BaseLayer.h"

#include <functional>

enum class GizmoMode : uint8_t
{
    Select = 0,
    Translate,
    Rotate,
    Scale,
    MAX,
};

enum class GizmoSpace : uint8_t
{
    World = 0,
    Local,
    MAX,
};

class ViewportLayer : public BaseLayer
{
public:
    ViewportLayer(View* view);

public:
    using GizmoCallback = std::function<void(const glm::mat4&, const glm::mat4&, const glm::vec3&, const glm::vec3&, const glm::vec3&)>;
    using FramebufferSizeCallback = std::function<void(double, double)>;

    uint32_t frameId = 0;
    uint32_t frameRenderId = 0;
    uint32_t frameTextureId = 0;

public:
    virtual void render() override;

public:
    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;
    bool wantCaptureGizmo() const;

    void remapToRelative(double& x, double& y);

    void setFramebufferSizeCallback(const FramebufferSizeCallback& callback);

public:
    bool getGizmoVisible() const;
    void setGizmoVisible(bool visible);

    GizmoMode getGizmoMode() const;
    GizmoSpace getGizmoSpace() const;

    void setGizmoMode(GizmoMode mode);
    void setGizmoSpace(GizmoSpace space);
    
    void setGizmoTransform(const glm::mat4& transform);
    void setGizmoCallback(const GizmoCallback& callback);

    void switchGizmoMode();
    void switchGizmoSpace();

private:
    void guizmo();
    void overlay();

private:
    glm::vec2 m_mouse;
    glm::vec2 m_position;

    glm::vec2 m_min;
    glm::vec2 m_max;

    uint32_t m_width;
    uint32_t m_height;

    bool m_wantCaptureMouse = false;
    bool m_wantCaptureKeyboard = false;
    bool m_wantCaptureGizmo = false;

    FramebufferSizeCallback m_sizeCallback;

private:
    bool m_gizmoVisible = false;
    
    glm::mat4 m_gizmoTransform{ 1.0f};
    glm::mat4 m_gizmoDelta{ 1.0f};

    glm::vec3 m_gizmoTranslation{ 1.0f };
    glm::vec3 m_gizmoRotation{ 1.0f };
    glm::vec3 m_gizmoScale{ 1.0f };

    GizmoMode m_gizmoMode = GizmoMode::Translate;
    GizmoSpace m_gizmoSpace = GizmoSpace::World;

    GizmoCallback m_gizmoCallback;
};