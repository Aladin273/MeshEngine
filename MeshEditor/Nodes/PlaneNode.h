#pragma once

#include "MeshEngine/Model/MeshNode.h"

class PlaneNode : public MeshNode
{
public:
	PlaneNode(glm::vec3 worldUp, float width, float height, uint32_t numSubs);
};