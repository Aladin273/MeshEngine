#include "Editor/Editor.h"

#include "MeshEngine/Misc/Settings.h"

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

    const uint8_t count = 10;
    const float step = (MeshEngine::Settings::bounds * 2.f) / count;
    const float size = step * 0.33f;

    for (uint8_t x = 0; x < count; ++x)
    {
        for (uint8_t y = 0; y < count; ++y)
        {
            for (uint8_t z = 0; z < count; ++z)
            {
                std::unique_ptr<MeshNode> cube = std::make_unique<MeshNode>();

                cube->setName(std::string("Cube ") + std::to_string(x) + " " + std::to_string(y) + " " + std::to_string(z));
                cube->attachMesh(Mesh::createCube(glm::vec3(0.f), size));
                cube->setRelativeTransform(glm::translate(glm::vec3(-MeshEngine::Settings::bounds + step * x, -MeshEngine::Settings::bounds + step * y, -MeshEngine::Settings::bounds + step * z)));

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

    std::unique_ptr<Scene> scene = defaultScene();
    //std::unique_ptr<Scene> scene = octreeScene();

    view->setScene(scene.get());

    return editor->start();
}
