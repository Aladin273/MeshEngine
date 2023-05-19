#include "Triad.h"

Triad::Triad()
{
	{
		Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 1,0,0 }));
		Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 0,1,0 }));
		Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 0,0,1 }));

		Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 1,0,0 }));
		Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 0,1,0 }));
		Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 0,0,1 }));

		Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 1,1,1 }));
		Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 1,1,1 }));
		Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 1,1,1 }));
		Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 1,1,1 }));
	}
}
