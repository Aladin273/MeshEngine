#pragma once

#include "LightNode.h"

#include "MeshEngine/Base/Light.h"
#include "MeshEngine/Base/Mesh.h"

class DirLightNode : public LightNode
{
public:
    DirLightNode();
    virtual ~DirLightNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Color3, "ambient", m_light.ambient);
        bindPropertyEx(Property::Color3, "diffuse", m_light.diffuse);
        bindPropertyEx(Property::Color3, "specular", m_light.specular);

        super::bind();
    }

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    DirLight m_light;
    uint32_t m_id = 0;
};
