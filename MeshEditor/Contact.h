#pragma once

#include "HalfEdge.h"
#include "Node.h"

struct Contact
{
    Utils::FaceHandle face;
    Node* node;
    float distance;
    glm::vec3 point;
};
