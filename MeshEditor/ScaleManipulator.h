#pragma once

#include "Manipulator.h"

class ScaleManipulator : public Manipulator
{
public:
	ScaleManipulator(glm::vec3 dir_L);
	void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) override;
	void setDirection(glm::vec3 inDir_L);

private:
	glm::vec3 m_centerPoint_L{ 0.0f };
	glm::vec3 m_startPoint_L{ 0.0f };
	glm::vec3 m_dir_L{ 0.0f, 0.0f, 1.0f };
};

