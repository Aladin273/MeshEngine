#include "TransformNodeOperator.h"

void TransformNodeOperator::onEnter(View& view)
{
	m_idle = true;
}

void TransformNodeOperator::onExit(View& view)
{
    clear();
}

void TransformNodeOperator::onMouseMove(View& view, double x, double y)
{
	if (m_edit)
		m_manipulator->handleMovement(MovementType::Drag, view.getViewport(), x, y);
}

void TransformNodeOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (button == m_button && action == Action::Press && m_idle == true)
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

        if (contacts.empty())
            return;

        auto manipulator = std::find_if(contacts.begin(), contacts.end(), [](Contact& contact)
            {
                return dynamic_cast<Manipulator*>(contact.node) != nullptr;
            });
        
        Contact& contact = manipulator != contacts.end() ? *manipulator : contacts.front();
        Node* node = contact.node;

        if (dynamic_cast<Manipulator*>(node) != nullptr)
        {
            m_idle = false;
            m_edit = true;
            m_manipulator = dynamic_cast<Manipulator*>(node);
            m_manipulator->handleMovement(MovementType::Push, view.getViewport(), x, y);
        }
        else if (dynamic_cast<Node*>(node) != nullptr)
        {
            m_view = &view;
            m_contact = contact;          
            clear();
            init();
            setupTriad();
            view.decorateTriad(*m_triad);
        }
    }
    else if (button == m_button && action == Action::Release && m_idle == false)
    {
        m_idle = true;
        m_edit = false;
        m_manipulator->handleMovement(MovementType::Release, view.getViewport(), x, y);
    }
}

void TransformNodeOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
}

void TransformNodeOperator::init()
{
    const auto& bbox = m_contact.node->getMesh()->getBoundingBox();
    m_center = (bbox.min + bbox.max) / 2.f;
}

void TransformNodeOperator::clear()
{
    if (m_triad)
    {
        m_triad->deleteFromParent();
        m_view->getModel()->detachNode(m_triad);
    }

    m_triad = nullptr;
    m_manipulator = nullptr;
}

void TransformNodeOperator::setupTriad()
{
    std::unique_ptr<Triad> triad = std::make_unique<Triad>();

    for (auto& child : triad->getChildren())
        child->applyRelativeTransform(m_contact.node->getRelativeTransform());

    triad->setRelativeTransform(glm::translate(m_center) * glm::inverse(m_contact.node->getRelativeTransform()));
    triad->setCallback([&](const glm::mat4& delta)
        {
            glm::vec3 centerNode = m_contact.node->getRelativeTransform() * glm::vec4(m_center, 1.0f);

            m_contact.node->applyRelativeTransform(glm::translate(-centerNode));
            m_contact.node->applyRelativeTransform(delta);
            m_contact.node->applyRelativeTransform(glm::translate(centerNode));
    
            // Remake
            auto& mat = m_triad->getChildren().front()->getRelativeTransform();
            glm::vec3 scale = { glm::length(mat[0]), glm::length(mat[1]), glm::length(mat[2]) };
            m_triad->setRelativeTransform(glm::translate(m_center) * glm::scale(scale) * glm::inverse(mat));
        });

    m_triad = triad.get();
    m_contact.node->attachNode(std::move(triad));
}
