#include "Sprite.h"

Sprite::Sprite()
{
    m_name = "Sprite";
    updateData();
}

Sprite::Sprite(const Material& material) :
    m_material(material)
{
    m_name = "Sprite";
    updateData();
}

void Sprite::updateData()
{
    float scaledWidth = m_material.diffuseMap.width;
    float scaledHeight = m_material.diffuseMap.height;

    m_renderVertices =
    {
        { { -scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
        { {  scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
        { {  scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
        { { -scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
    };

    m_renderTriangles =
    {
        0, 1, 2,
        2, 3, 0,
    };

    m_renderLines =
    {
        0, 1,
        1, 2,
        2, 3,
        3, 0,
    };

    m_bbox.min.x = -scaledWidth;
    m_bbox.min.y = -scaledHeight;
    
    m_bbox.max.x = scaledWidth;
    m_bbox.max.y = scaledHeight;

    m_renderDataDirty = true;
}

const BoundingBox& Sprite::getBoundingBox() const
{
    return m_bbox;
}

const Material& Sprite::getMaterial() const
{
    return m_material;
}

void Sprite::setMaterial(const Material& material)
{
    m_material = material;
}

float Sprite::getPixelSize() const
{
    return m_pixelSize;
}

void Sprite::setPixelSize(float pixelSize)
{
    m_pixelSize = pixelSize;
}
