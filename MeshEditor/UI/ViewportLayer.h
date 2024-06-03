#pragma once

#include "BaseLayer.h"

class ViewportLayer : public BaseLayer
{
public:
    ViewportLayer(View* view);

    virtual void attach(uint32_t textureId, uint32_t width, uint32_t height);

    virtual void render() override;

    glm::vec2 getSize() const;
    glm::vec2 getPosition() const;

    glm::vec2 getMin() const;
    glm::vec2 getMax() const;

private:
    uint32_t m_textureId;
    uint32_t m_width;
    uint32_t m_height;

    glm::vec2 m_size;
    glm::vec2 m_position;

    glm::vec2 m_min;
    glm::vec2 m_max;
};