#include "Editor/Editor.h"

int main()
{    
    Editor* editor = Editor::instance();
    View* view = editor->createView(MeshEngine::Settings::title, MeshEngine::Settings::width, MeshEngine::Settings::height, MeshEngine::Settings::icon);

    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    std::unique_ptr<MeshNode> floor = std::make_unique<MeshNode>();

    floor->setName("Cube");

    floor->attachMesh(Mesh::createCube(glm::vec3(0.f), 1.f));
    floor->setRelativeTransform(glm::scale(glm::mat4(1.f), glm::vec3(20.f, 1.f, 20.f)));

    scene->attachNode(std::move(floor));

    view->setScene(scene.get());
    editor->run();

    return 0;
}
