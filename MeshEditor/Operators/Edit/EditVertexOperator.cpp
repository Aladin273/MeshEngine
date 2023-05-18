#include "EditVertexOperator.h"

void EditVertexOperator::onEnter(View&)
{
    m_idle = true;
}

void EditVertexOperator::onExit(View&)
{
    clear();
}

void EditVertexOperator::onMouseMove(View& view, double x, double y)
{
    // Edit On
    if (m_edit)
        m_manipulator->handleMovement(MovementType::Drag, view.getViewport(), x, y);
}

void EditVertexOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
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

void EditVertexOperator::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{

}

void EditVertexOperator::clear()
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


void EditVertexOperator::init()
{
    const auto& table = m_contact.node->getMesh()->getHalfEdgeTable();
    Utils::HalfEdgeHandle start_heh = table.deref(m_contact.face).heh;
    Utils::HalfEdgeHandle next_heh = start_heh;
    std::vector<glm::vec3> normals;

    glm::vec3 point = glm::inverse(m_contact.node->calcAbsoluteTransform()) * glm::vec4(m_contact.point, 1.0f);

    float min = glm::length(table.getEndPoint(start_heh) - point);
    m_vh = table.deref(start_heh).dst;

    // Find needed vertex
    do
    {
        float length = glm::length(table.getEndPoint(next_heh) - point);

        if (length < min)
        {
            min = length;
            m_vh = table.deref(next_heh).dst;
        }
        next_heh = table.next(next_heh);
    } 
    while (next_heh != start_heh);
   
    start_heh = table.deref(m_vh).heh;
    next_heh = start_heh;

    // Find adjacent normals
    do
    {
        Utils::HalfEdgeHandle heh0 = next_heh;
        Utils::HalfEdgeHandle heh1 = table.next(heh0);
        Utils::HalfEdgeHandle heh2 = table.next(heh1);

        glm::vec3 a = table.getEndPoint(heh0);
        glm::vec3 b = table.getEndPoint(heh1);
        glm::vec3 c = table.getEndPoint(heh2);

        normals.push_back(glm::normalize(glm::cross(b - a, c - b)));
        next_heh = table.next(table.twin(next_heh));
    } 
    while (next_heh != start_heh);

    m_normal = glm::vec3(0.0f);

    for (auto& normal : normals)
        m_normal += normal;

    m_center = table.getPoint(m_vh);
    m_normal = glm::normalize(m_normal / static_cast<float>(normals.size()));
}

void EditVertexOperator::setupTriad()
{
    std::unique_ptr<Triad> triad = std::make_unique<Triad>();

    triad->setRelativeTransform(glm::translate(m_center));
    triad->setCallback([&](const glm::mat4& delta)
        {
            m_contact.node->getMesh()->applyTransformation(m_vh, delta);
        });

    m_triad = triad.get();
    m_contact.node->attachNode(std::move(triad));
}

void EditVertexOperator::setupArrow()
{
    std::unique_ptr<TranslationManipulator> manipulator = std::make_unique<TranslationManipulator>(m_normal);

    manipulator->setRelativeTransform(glm::translate(m_center));
    manipulator->setCallback([&](const glm::mat4& delta)
        {
            m_contact.node->getMesh()->applyTransformation(m_vh, delta);
        });

    m_manipulator = manipulator.get();
    m_contact.node->attachNode(std::move(manipulator));
}

