#include "Application/Application.h"

int main()
{    
    Application* app = Application::instance();
    View* view = app->createView(MeshEngine::Settings::title, MeshEngine::Settings::width, MeshEngine::Settings::height, MeshEngine::Settings::icon);

    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    std::unique_ptr<MeshNode> cube = std::make_unique<MeshNode>();

    cube->setName("Cube");
    cube->attachMesh(Mesh::createCube(glm::vec3(0.f), 1.f));
    
    scene->attachNode(std::move(cube));

    view->setScene(scene.get());
    app->run();

    return 0;
}
