#pragma once

#include "MeshEngine/Node/Node.h"
#include "MeshEngine/Math/HalfEdge.h"

struct Contact
{
    HalfEdgeFaceHandle face;
    Node* node;
    float distance;
    glm::vec3 point;
};

enum class FilterValue
{
    Node = 1,
    Manipulator = 2,
    NM = (Node | Manipulator),
};
