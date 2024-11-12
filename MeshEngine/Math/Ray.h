#pragma once

#include "glm/vec3.hpp"

struct Ray
{
    glm::vec3 orig{ 0 };
    glm::vec3 dir{ 0,0,1 };
};