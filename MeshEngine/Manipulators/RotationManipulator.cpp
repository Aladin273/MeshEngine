#include "RotationManipulator.h"

#include "ThirdParty/glm/gtx/intersect.hpp"
#include "ThirdParty/glm/gtx/vector_angle.hpp"

RotationManipulator::RotationManipulator(glm::vec3 dir_L)
{
	setDirection(dir_L);
}

void RotationManipulator::handleMovement(MovementType movementType, const Viewport& viewport, double x, double y)
{
	const glm::mat4& mat = calcAbsoluteTransform();
	const glm::mat4& inverse = glm::inverse(mat);

	ray cursorRay_W = viewport.calcCursorRay(x, y);
	ray plane_W = { mat * glm::vec4(m_centerPoint_L, 1.0f), glm::normalize(mat * glm::vec4(m_dir_L, 0.0f)) };

	if (movementType == MovementType::Push)
	{
		glm::vec3 startPoint_W;
		if (glm::intersectRayPlane(plane_W.orig, plane_W.dir, cursorRay_W.orig, cursorRay_W.dir, startPoint_W))
			m_startPoint_L = inverse * glm::vec4(startPoint_W, 1.0f);
	}
	else if (movementType == MovementType::Drag || movementType == MovementType::Release)
	{
		glm::vec3 closestPoint_W;

		if (glm::intersectRayPlane(plane_W.orig, plane_W.dir, cursorRay_W.orig, cursorRay_W.dir, closestPoint_W))
		{
			glm::vec3 closestPoint_L = inverse * glm::vec4(closestPoint_W, 1.0f);
			glm::vec3 dir_R = glm::normalize(getRelativeTransform() * glm::vec4(m_dir_L, 0.0f));

			glm::vec3 startPoint = glm::normalize(m_startPoint_L);
			glm::vec3 closestPoint = glm::normalize(closestPoint_L);

			float sign = glm::dot(m_dir_L, glm::cross(startPoint, closestPoint)) > 0 ? 1 : -1;
			float angle = glm::radians(sign * glm::angle(startPoint, closestPoint));

			sendFeedback(glm::rotate(angle, dir_R));
		}
	}
}

void RotationManipulator::setDirection(glm::vec3 inDir_L)
{
	m_dir_L = inDir_L;
}
