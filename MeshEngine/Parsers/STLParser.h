#pragma once

#include <string>
#include <vector>

#include "MeshEngine/Model/Model.h"
#include "MeshEngine/Model/MeshNode.h"

class STLParser
{
public:
    static constexpr double epsilon = 1e-8;

    struct Vec
    {
        double x, y, z;

        bool operator<(const Vec& b) const;
        bool operator>(const Vec& b) const;
        bool operator==(const Vec& b) const;
        bool operator!=(const Vec& b) const;
    };

    struct Triangle
    {
        Vec A, B, C;
        Vec normal;
    };

    using TriangleSoup = std::vector<Triangle>;

    TriangleSoup read(const std::string& filename);
    void write(const TriangleSoup& soup, const std::string& filename);

    std::unique_ptr<MeshNode> loadNode(const std::string& filename);
    std::unique_ptr<Model> loadModel(const std::string& filename);
    void saveNode(TriangleSoup& soup, MeshNode* node);
    void saveModel(const Model& model, const std::string& filename);

    static bool approximatelyEqual(double a, double b, double epsilon);
};




