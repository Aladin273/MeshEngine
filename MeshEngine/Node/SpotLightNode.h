#pragma once

#include "LightNode.h"

#include "MeshEngine/Base/Mesh.h"

class SpotLightNode : public LightNode
{
public:
    SpotLightNode();
    virtual ~SpotLightNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Color, "ambient", ambient);
        bindPropertyEx(Property::Color, "diffuse", diffuse);
        bindPropertyEx(Property::Color, "specular", specular);

        bindProperty(cutOff);
        bindProperty(outerCutOff);
        bindProperty(constant);
        bindProperty(linear);
        bindProperty(quadratic);

        super::bind();
    }

public:
    glm::vec3 ambient{ 1.f };
    glm::vec3 diffuse{ 1.f };
    glm::vec3 specular{ 1.f };

    float cutOff = glm::cos(glm::radians(25.f));
    float outerCutOff = glm::cos(glm::radians(35.f));
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
