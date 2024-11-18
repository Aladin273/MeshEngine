#pragma once

#include "LightNode.h"

#include "MeshEngine/Base/Mesh.h"

class DirLightNode : public LightNode
{
public:
    DirLightNode();
    virtual ~DirLightNode();

public:
    virtual void bind() override
    {
        bindPropertyEx(Property::Color, "ambient", ambient);
        bindPropertyEx(Property::Color, "diffuse", diffuse);
        bindPropertyEx(Property::Color, "specular", specular);

        super::bind();
    }

public:
    glm::vec3 ambient{ 1.f };
    glm::vec3 diffuse{ 1.f };
    glm::vec3 specular{ 1.f };

public:
    virtual void start() override;
    virtual void end() override;

    virtual void update(float deltaTime) override;
    virtual void render(RenderSystem* renderSystem) override;

private:
    uint32_t m_id = 0;
};
