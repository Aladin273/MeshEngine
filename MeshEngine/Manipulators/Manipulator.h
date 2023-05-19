#pragma once

#include <functional>

#include "Viewport/Viewport.h"
#include "Model/Node.h"
#include "Math/Intersections.h"

enum class MovementType
{
	Push,
	Drag,
	Release
};

class Manipulator : public Node
{
public:
	virtual void handleMovement(MovementType movementType, const Viewport& viewport, double x, double y) {}
	void setCallback(const std::function<void(const glm::mat4&)>& inCallback);

protected:
	void sendFeedback(const glm::mat4& deltaMatrix);

private:
	bool m_movement = true;
	glm::vec3 m_centerPoint{ 0.0f };
	std::function<void(const glm::mat4&)> m_callback;
};

