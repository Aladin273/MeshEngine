#pragma once

#include <string>
#include <memory>

#include "MeshEngine/Node/Node.h"

#include "MeshEngine/Parser/ColladaParser.h"
#include "MeshEngine/Parser/STLParser.h"
#include "MeshEngine/Parser/AssimpParser.h"

class AssetSystem
{
public:
    void init();

    std::unique_ptr<Node> loadModel(const std::string& filename);
    void saveModel(Node& model, const std::string& filename);

    std::unique_ptr<Node> loadImage(const std::string& filename);
    void saveImage(Node& image, const std::string& filename);

private:
    std::unique_ptr<STLParser> m_stl;
    std::unique_ptr<ColladaParser> m_collada;
    std::unique_ptr<AssimpParser> m_assimp;
};