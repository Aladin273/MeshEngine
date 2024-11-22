#pragma once

#include "Base.h"
#include "Texture.h"

struct Material : public Base
{
    Material() { m_name = "Material"; };

    Material(const glm::vec3& ambient, const glm::vec4& diffuse, const glm::vec3& specular, const glm::vec3& emission, float shininess)
        : ambient(ambient), diffuse(diffuse), specular(specular), emission(emission), shininess(shininess) { m_name = "Material"; };

    virtual ~Material() {};

    virtual void bind() override
    {
        bindPropertyEx(Property::Color, "ambient", ambient);
        bindPropertyEx(Property::ColorEx, "diffuse", diffuse);
        bindPropertyEx(Property::Color, "specular", specular);
        bindPropertyEx(Property::Color, "emission", emission);
        bindProperty(shininess);

        bindProperty(diffuseMap);
        bindProperty(specularMap);
        bindProperty(emissionMap);

        super::bind();
    }

    glm::vec3 ambient{ 0.0f };
    glm::vec4 diffuse{ 1.0f };
    glm::vec3 specular{ 1.0f };
    glm::vec3 emission{ 0.0f };
    float shininess{ 128.0f };

    Texture diffuseMap;
    Texture specularMap;
    Texture emissionMap;
};