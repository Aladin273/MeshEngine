#include "EditFaceOperator.h"

void EditFaceOperator::onEnter(View&)
{
    m_idle = true;
}

void EditFaceOperator::onExit(View&)
{
    clear();
}

void EditFaceOperator::onMouseMove(View& view, double x, double y)
{
    // Edit On
    if (m_edit)
        m_manipulator->handleMovement(MovementType::Drag, view.getViewport(), x, y);
}

void EditFaceOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Press && m_idle == true) // Idle On
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
        
        if (dynamic_cast<Manipulator*>(node) != nullptr) // Selected Manipulator
        {
            m_idle = false;
            m_edit = true;
            m_manipulator = dynamic_cast<Manipulator*>(node);
            m_manipulator->handleMovement(MovementType::Push, view.getViewport(), x, y);
        }
        else if (dynamic_cast<Node*>(node) != nullptr) // Selected Node
        {
            m_view = &view;
            m_contact = contact;
       
            clear();
            init();

            if (button == m_buttonArrow)
            {
                setupArrow();
                view.decorateArrow(*m_manipulator, m_normal);
            }
                
            else if (button == m_buttonTriad)
            {
                setupTriad();
                view.decorateTriad(*m_triad);
            }
        }
    }
    else if ((button == m_buttonArrow || button == m_buttonTriad) && action == Action::Release && m_idle == false) // Idle Off
    {
        m_idle = true;
        m_edit = false;
        m_manipulator->handleMovement(MovementType::Release, view.getViewport(), x, y);
    }
}

void EditFaceOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}

void EditFaceOperator::clear()
{
    if (m_manipulator)
    {
        m_manipulator->deleteFromParent();
        m_view->getModel()->detachNode(m_manipulator);
    }

    if (m_triad)
    {
        m_triad->deleteFromParent();
        m_view->getModel()->detachNode(m_triad);
    }

    m_triad = nullptr;
    m_manipulator = nullptr;
}

void EditFaceOperator::init()
{
    const auto& table = m_contact.node->getMesh()->getHalfEdgeTable();
    heds::HalfEdgeHandle heh0 = table.deref(m_contact.face).heh;
    heds::HalfEdgeHandle heh1 = table.next(heh0);
    heds::HalfEdgeHandle heh2 = table.next(heh1);
    heds::HalfEdgeHandle heh3 = table.next(heh2);

    glm::vec3 a = table.getEndPoint(heh0);
    glm::vec3 b = table.getEndPoint(heh1);
    glm::vec3 c = table.getEndPoint(heh2);
    glm::vec3 d = table.getEndPoint(heh3);

    m_normal = glm::normalize(glm::cross(b - a, c - b));
  
    if (heh3 == heh0)
        m_center = (a + b + c) / 3.0f;
    else
        m_center = (a + b + c + d) / 4.0f;
}

void EditFaceOperator::setupTriad()
{
    std::unique_ptr<Triad> triad = std::make_unique<Triad>();

    triad->setRelativeTransform(glm::translate(m_center));
    triad->setCallback([&](const glm::mat4& delta)
        {
            m_contact.node->getMesh()->applyTransformation(m_contact.face, delta);
        });

    m_triad = triad.get();
    m_contact.node->attachNode(std::move(triad));
}

void EditFaceOperator::setupArrow()
{
    std::unique_ptr<TranslationManipulator> manipulator = std::make_unique<TranslationManipulator>(m_normal);

    manipulator->setRelativeTransform(glm::translate(m_center));
    manipulator->setCallback([&](const glm::mat4& delta)
        {
            m_contact.node->getMesh()->applyTransformation(m_contact.face, delta);
        });

    m_manipulator = manipulator.get();
    m_contact.node->attachNode(std::move(manipulator));
}
