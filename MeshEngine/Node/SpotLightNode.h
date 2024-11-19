#pragma once

#include "LightNode.h"

#include "MeshEngine/Base/Light.h"
#include "MeshEngine/Base/Mesh.h"

class SpotLightNode : public LightNode
{
public:
    SpotLightNode();
    virtual ~SpotLightNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Color, "ambient", m_light.ambient);
        bindPropertyEx(Property::Color, "diffuse", m_light.diffuse);
        bindPropertyEx(Property::Color, "specular", m_light.specular);

        bindPropertyEx(Property::Float, "constant", m_light.constant);
        bindPropertyEx(Property::Float, "linear", m_light.linear);
        bindPropertyEx(Property::Float, "quadratic", m_light.quadratic);

        bindPropertyEx(Property::Float, "cutOff", m_light.cutOff);
        bindPropertyEx(Property::Float, "outerCutOff", m_light.outerCutOff);

        super::bind();
    }

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    SpotLight m_light;
    uint32_t m_id = 0;
};
