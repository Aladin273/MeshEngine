#include "AssetSystem.h"

#include "MeshEngine/Misc/Logger.h"
#include "MeshEngine/Misc/Settings.h"

#include "MeshEngine/Node/MeshNode.h"
#include "MeshEngine/Node/SpriteNode.h"

void AssetSystem::init()
{
    m_stl = std::make_unique<STLParser>();
    m_collada = std::make_unique<ColladaParser>();
    m_assimp = std::make_unique<AssimpParser>();

    MeshEngine::Logger::info("AssetSystem inited successful");
}

std::unique_ptr<Node> AssetSystem::loadModel(const std::string& filename)
{
    if (filename.find(".stl") != MeshEngine::Settings::invalid)
        return m_stl->loadModel(filename);
    else if (filename.find(".dae") != MeshEngine::Settings::invalid)
        return m_collada->loadModel(filename);
    else
        return m_assimp->loadModel(filename);
}

void AssetSystem::saveModel(Node& model, const std::string& filename)
{
    if (filename.find(".stl") != MeshEngine::Settings::invalid)
        m_stl->saveModel(model, filename);
    else if (filename.find(".dae") != MeshEngine::Settings::invalid)
        m_collada->saveModel(model, filename);
    //else
    //    m_assimp->saveMode(model, filename);
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