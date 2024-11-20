#include "DeleteFaceOperator.h"

void DeleteFaceOperator::onEnter(View&)
{
}

void DeleteFaceOperator::onExit(View&)
{
}

void DeleteFaceOperator::onMouseMove(View& view, double x, double y)
{
}

void DeleteFaceOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (button == ButtonCode::Button_Left && action == Action::Press)
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

        if (contacts.empty())
            return;

        Contact& contact = contacts.front();
        MeshNode* node = dynamic_cast<MeshNode*>(contact.node);

        if (node)
        {
            Mesh* mesh = node->getMesh();

            if (mesh)
                mesh->deleteFace(contact.face);

            node->reset();
        }
    }
}


void DeleteFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
}
