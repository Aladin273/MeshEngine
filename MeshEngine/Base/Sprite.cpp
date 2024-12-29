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

void Sprite::propertyChanged(const Property& property)
{
    updateSubData();
}

void Sprite::updateData()
{
    float scaledWidth = m_material.diffuseMap.width * m_pixelSize;
    float scaledHeight = m_material.diffuseMap.height * m_pixelSize;

    m_renderVertices =
    {
        { { -scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
        { {  scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
        { {  scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
        { { -scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
    };

    m_renderTriangles = { 0, 1, 2, 2, 3, 0 };

    m_bbox.setMin(glm::vec3(-scaledWidth, -scaledHeight, -0.5f));
    m_bbox.setMax(glm::vec3(scaledWidth, scaledHeight, 0.5f));

    m_renderDataDirty = true;

    super::updateData();
}

void Sprite::updateSubData()
{
    float scaledWidth = m_material.diffuseMap.width * m_pixelSize;
    float scaledHeight = m_material.diffuseMap.height * m_pixelSize;

    m_renderVertices =
    {
        { { -scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 1.f } },
        { {  scaledWidth,  scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 1.f } },
        { {  scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 1.f, 0.f } },
        { { -scaledWidth, -scaledHeight, 0.f }, { 0.f, 0.f, 1.f }, { 0.f, 0.f } },
    };

    m_renderSubData = { 0, 1, 2, 3 };

    m_bbox.setMin(glm::vec3(-scaledWidth, -scaledHeight, -0.5f));
    m_bbox.setMax(glm::vec3(scaledWidth, scaledHeight, 0.5f));

    m_renderSubDataDirty = true;

    super::updateSubData();
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
    updateData();
}

float Sprite::getPixelSize() const
{
    return m_pixelSize;
}

void Sprite::setPixelSize(float pixelSize)
{
    m_pixelSize = pixelSize;
    updateSubData();
}
