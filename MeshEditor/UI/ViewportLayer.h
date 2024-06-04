#pragma once

#include "BaseLayer.h"

#include <functional>

class ViewportLayer : public BaseLayer
{
public:
    using FramebufferSizeCallback = std::function<void(double, double)>;

    ViewportLayer(View* view);

    virtual void render() override;

    void attach(uint32_t textureId);

    bool wantCaptureMouse() const;
    bool wantCaptureKeyboard() const;

    void remapToRelative(double& x, double& y);

    void setFramebufferSizeCallback(const FramebufferSizeCallback& callback);

private:
    void viewport();
    void overlay();

    uint32_t m_textureId;

    uint32_t m_width;
    uint32_t m_height;

    glm::vec2 m_mouse;
    glm::vec2 m_position;
    
    glm::vec2 m_min;
    glm::vec2 m_max;

    bool m_wantCaptureMouse;
    bool m_wantCaptureKeyboard;

    std::vector<FramebufferSizeCallback> m_sizeCallbacks;
};