#include "Application/Application.h"

int main()
{    
    Application* app = Application::instance();
    View* view = app->createView(Settings::title, Settings::width, Settings::height, Settings::icon);

    std::unique_ptr<Model> model = std::make_unique<Model>();
    
    std::unique_ptr<Node> nodeFloor = std::make_unique<Node>();

    nodeFloor->attachMesh(Mesh::createCube(glm::vec3(0.f), 1.f));
    nodeFloor->setRelativeTransform(glm::translate(glm::vec3(0, -1, 0)) * glm::scale(glm::vec3(100.f, 1.f, 100.0f)));

    std::unique_ptr<Node> nodeCube = std::make_unique<Node>();

    nodeCube->attachMesh(Mesh::createCube(glm::vec3(0.f), 1.f));

    //model->attachNode(std::move(nodeFloor));
    model->attachNode(std::move(nodeCube));

    view->setModel(model.get());
    app->run();

    return 0;
}
