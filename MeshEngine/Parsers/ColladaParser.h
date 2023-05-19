#pragma once

#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <functional>

#include "ThirdParty/tinyxml2/tinyxml2.h"
#include "Model/Model.h"

class ColladaParser
{
    enum class Primitives
    {
        Triangles,
        Polylist,
    };

    struct Geometry
    {
        std::map<std::string, std::vector<float>> sources;
        std::vector<int> vcounts;
        std::vector<int> indices;
        Primitives type;
    };

    using GeometryMap = std::map<std::string, Geometry>;

    const std::map<std::string, std::function<glm::mat4(std::vector<float>&)>> transforms
    { 
        {"matrix", [](std::vector<float>& data) { return glm::transpose(glm::make_mat4(data.data())); }},
        {"translate", [](std::vector<float>& data) { return glm::translate(glm::make_vec3(data.data())); }},
        {"rotate", [](std::vector<float>& data) { return glm::rotate(data.back(), glm::make_vec3(data.data())); }},
        {"scale", [](std::vector<float>& data) { return glm::scale(glm::make_vec3(data.data())); }}
    };

    void saveNode(Node* parent, tinyxml2::XMLElement* pParent, tinyxml2::XMLElement* pLib);
    std::unique_ptr<Node> loadNode(Node* parent, tinyxml2::XMLElement* pNode, const GeometryMap& geometries);

public:
    std::unique_ptr<Model> loadModel(const std::string& filename);
    void saveModel(const Model& model, const std::string& filename);
};

