#pragma once

#include "LightNode.h"

#include "MeshEngine/Base/Mesh.h"

class PointLightNode : public LightNode
{
public:
    PointLightNode();
    virtual ~PointLightNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Color, "ambient", ambient);
        bindPropertyEx(Property::Color, "diffuse", diffuse);
        bindPropertyEx(Property::Color, "specular", specular);

        bindProperty(constant);
        bindProperty(linear);
        bindProperty(quadratic);

        super::bind();
    }

public:
    glm::vec3 ambient{ 1.f };
    glm::vec3 diffuse{ 1.f };
    glm::vec3 specular{ 1.f };

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    uint32_t m_id = 0;
};
