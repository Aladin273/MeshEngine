#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "Model.h"

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

    std::unique_ptr<Node> loadNode(const std::string& filename);
    std::unique_ptr<Model> loadModel(const std::string& filename);
    void saveNode(TriangleSoup& soup, Node* node);
    void saveModel(const Model& model, const std::string& filename);

    static bool approximatelyEqual(double a, double b, double epsilon);
};




