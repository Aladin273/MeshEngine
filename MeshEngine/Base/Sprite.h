#pragma once

#include "MeshEngine/Math/BoundingBox.h"

#include "MeshEngine/Base/RenderBase.h"
#include "MeshEngine/Base/Texture.h"
#include "MeshEngine/Base/Material.h"

class Sprite : public RenderBase
{
public:
    Sprite();
    Sprite(const Material& material);

public:
    virtual void bind() override
    {
        bindProperty(m_material);
        bindProperty(m_pixelSize);

        super::bind();
    };

public:
    virtual void updateData() override;

public:
    const BoundingBox& getBoundingBox() const;

    const Material& getMaterial() const;
    void setMaterial(const Material& material);

    float getPixelSize() const;
    void setPixelSize(float pixelSize);

private:
    BoundingBox m_bbox;
    Material m_material;
    float m_pixelSize = 0.05f;
};