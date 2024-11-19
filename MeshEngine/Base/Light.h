#pragma once

#include "Base.h"

struct DirLight
{
    glm::vec3 direction;
    float padding1;

    glm::vec3 ambient{ 1.f };
    float padding2;

    glm::vec3 diffuse{ 1.f };
    float padding3;

    glm::vec3 specular{ 1.f };
    float padding4;
};

struct PointLight
{
    glm::vec3 position;
    float padding1;

    float constant = 1.0f;
    float linear = 0.09f;
    float quadratic = 0.032f;
    float padding2;

    glm::vec3 ambient{ 1.f };
    float padding3;

    glm::vec3 diffuse{ 1.f };
    float padding4;

    glm::vec3 specular{ 1.f };
    float padding5;
};

struct SpotLight
{
    glm::vec3 position;
    float padding1;

    glm::vec3 direction;
    float padding2;

    float cutOff = glm::cos(glm::radians(25.f));
    float outerCutOff = glm::cos(glm::radians(35.f));
    float constant = 1.0f;
    float linear = 0.09f;

    glm::vec3 padding3;
    float quadratic = 0.032f;

    glm::vec3 ambient{ 1.f };
    float padding4;

    glm::vec3 diffuse{ 1.f };
    float padding5;

    glm::vec3 specular{ 1.f };
    float padding6;
};