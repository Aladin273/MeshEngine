#include "Editor/Editor.h"

#include "MeshEngine/Node/MeshNode.h"
#include "MeshEngine/Node/DirLightNode.h"

int main()
{    
    Editor* editor = Editor::instance();
    View* view = editor->createView(MeshEngine::Settings::title, MeshEngine::Settings::width, MeshEngine::Settings::height, MeshEngine::Settings::icon);

    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    
    std::unique_ptr<MeshNode> floor = std::make_unique<MeshNode>();
    std::unique_ptr<DirLightNode> sunLight = std::make_unique<DirLightNode>();
    
    sunLight->setName("SunLight");
    sunLight->setRelativeTransform(glm::translate(glm::vec3(10.f, 10.f, 0.f)) * glm::rotate(glm::radians(60.f), glm::vec3(1.f, -0.25f, -1.f)));

    floor->setName("Floor");
    floor->attachMesh(Mesh::createCube(glm::vec3(0.f), 1.f));
    floor->setRelativeTransform(glm::scale(glm::mat4(1.f), glm::vec3(25.f, 1.f, 25.f)));

    scene->attachNode(std::move(floor));
    scene->attachNode(std::move(sunLight));

    view->setScene(scene.get());

    return editor->start();
}
