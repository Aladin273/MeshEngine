#include "ScaleManipulator.h"
#include <iostream>

ScaleManipulator::ScaleManipulator(glm::vec3 dir_L)
{
    setDirection(dir_L);
}

void ScaleManipulator::handleMovement(MovementType movementType, const Viewport& viewport, double x, double y)
{
	const glm::mat4& mat = calcAbsoluteTransform();
	const glm::mat4& inverse = glm::inverse(mat);

	ray cursorRay_W = viewport.calcCursorRay(x, y);
	ray ray_W = { mat * glm::vec4(m_centerPoint_L, 1.0f), glm::normalize(mat * glm::vec4(m_dir_L, 0.0f)) };

	if (movementType == MovementType::Push)
	{
		glm::vec3 startPoint_W;

		if (Utils::intersectRayRay(cursorRay_W.orig, cursorRay_W.dir, ray_W.orig, ray_W.dir, startPoint_W))
			m_startPoint_L = inverse * glm::vec4(startPoint_W, 1.0f);
	}
	else if (movementType == MovementType::Drag || movementType == MovementType::Release)
	{
		glm::vec3 closestPoint_W;

		if (Utils::intersectRayRay(cursorRay_W.orig, cursorRay_W.dir, ray_W.orig, ray_W.dir, closestPoint_W))
		{
			glm::vec3 closestPoint_L = inverse * glm::vec4(closestPoint_W, 1.0f);
			
			glm::vec3 delta = glm::vec3(1.0f) + glm::normalize(closestPoint_L - m_startPoint_L);

			if (delta.x > 1.0f)
				sendFeedback(glm::scale(glm::vec3(1.01f)));
			else
				sendFeedback(glm::scale(glm::vec3(0.99f)));

			//sendFeedback(glm::scale(delta));
		}	
	}
}

void ScaleManipulator::setDirection(glm::vec3 inDir_L)
{
	m_dir_L = inDir_L;
}
