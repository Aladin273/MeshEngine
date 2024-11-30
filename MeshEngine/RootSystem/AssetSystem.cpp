#include "AssetSystem.h"

#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Node/MeshNode.h"
#include "MeshEngine/Node/SpriteNode.h"

#include <glm/gtx/matrix_decompose.hpp>

void AssetSystem::init()
{
    m_stl = std::make_unique<STLParser>();
    m_collada = std::make_unique<ColladaParser>();
    m_assimp = std::make_unique<AssimpParser>();

    MeshEngine::Logger::info("AssetSystem inited successful");
}

std::unique_ptr<Node> AssetSystem::loadModel(const std::string& filename)
{
    std::unique_ptr<Node> model;

    if (filename.find(".stl") != MeshEngine::Settings::invalid)
        model = m_stl->loadModel(filename);
    else if (filename.find(".dae") != MeshEngine::Settings::invalid)
        model = m_collada->loadModel(filename);
    else
        model = m_assimp->loadModel(filename);

    // Re-center mesh
    model->processRecursive([](Node& node) -> bool
        {
            if (MeshNode* meshNode = dynamic_cast<MeshNode*>(&node))
            {
                glm::vec3 center = glm::vec4((meshNode->getBoundingBox().min + meshNode->getBoundingBox().max) / 2.0f, 1.0f);

                if (glm::any(glm::notEqual(glm::vec3(0.0f), center, 1e-8)))
                {
                    auto& table = meshNode->getMesh()->getHalfEdgeTable();

                    for (auto& vertex : table.getVertices())
                        vertex.data.position -= center;

                    meshNode->getMesh()->updateData();
                    meshNode->applyRelativeTransform(glm::translate(center));

                    meshNode->reset();
                }
            }

            return true;
        });

    // Re-center node
    glm::vec3 translation, scale, skew;
    glm::vec4 perspective; glm::quat rotation;

    glm::decompose(model->getRelativeTransform(), scale, rotation, translation, skew, perspective);
    model->applyRelativeTransform(glm::translate(-translation));

    return model;
}

void AssetSystem::saveModel(Node& model, const std::string& filename)
{
    // TODO ???
}

std::unique_ptr<Node> AssetSystem::loadImage(const std::string& filename)
{
    Material material;
    material.diffuseMap.path = filename;

    std::unique_ptr<SpriteNode> spriteNode = std::make_unique<SpriteNode>();
    spriteNode->attachSprite(std::make_unique<Sprite>(material));
    spriteNode->setName(filename);

    return spriteNode;
}

void AssetSystem::saveImage(Node& image, const std::string& filename)
{
    // TODO ???
}