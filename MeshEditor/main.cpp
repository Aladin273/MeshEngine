#include "Editor/Editor.h"

#include "MeshEngine/Node/MeshNode.h"
#include "MeshEngine/Node/DirLightNode.h"

std::unique_ptr<Scene> defaultScene()
{
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

    return scene;
}

std::unique_ptr<Scene> octreeScene()
{
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();

    std::unique_ptr<DirLightNode> sunLight = std::make_unique<DirLightNode>();
    
    sunLight->setName("SunLight");
    sunLight->setRelativeTransform(glm::translate(glm::vec3(100000.f, 100000.f, 0.f)) * glm::rotate(glm::radians(60.f), glm::vec3(1.f, -0.25f, -1.f)));

    scene->attachNode(std::move(sunLight));

    for (size_t x = 0; x < 24; ++x)
    {
        for (size_t y = 0; y < 24; ++y)
        {
            for (size_t z = 0; z < 24; ++z)
            {
                std::unique_ptr<MeshNode> cube = std::make_unique<MeshNode>();

                cube->setName(std::string("Cube ") + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z));
                cube->attachMesh(Mesh::createCube(glm::vec3(0.f), 50.f));
                cube->setRelativeTransform(glm::translate(glm::vec3(-10000.f + 200.f * x, -10000.f + 200.f * y, -10000.f + 200.f * z)));

                scene->attachNode(std::move(cube));
            }
        }
    }

    return scene;
}

int main()
{    
    Editor* editor = Editor::instance();
    View* view = editor->createView(MeshEngine::Settings::title, MeshEngine::Settings::width, MeshEngine::Settings::height, MeshEngine::Settings::icon);

    //std::unique_ptr<Scene> scene = defaultScene();
    std::unique_ptr<Scene> scene = octreeScene();

    view->setScene(scene.get());

    return editor->start();
}
