#pragma once

#include "MeshEngine/Math/HalfEdge.h"
#include "MeshEngine/Node/Node.h"

enum class FilterValue
{
    Node = 1,
    Manipulator = 2,
    NM = (Node | Manipulator),
};

struct Contact
{
    heds::FaceHandle face;
    Node* node;
    float distance;
    glm::vec3 point;
};
