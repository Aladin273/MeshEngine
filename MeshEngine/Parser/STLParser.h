#pragma once

#include <string>
#include <vector>

#include "MeshEngine/Node/Node.h"

class STLParser
{
public:
    std::unique_ptr<Node> loadModel(const std::string& filename);
    void saveModel(Node& model, const std::string& filename);

private:
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

    std::unique_ptr<Node> loadNode(const std::string& filename);
    void saveNode(std::vector<Triangle>& soup, Node* node);

    std::vector<Triangle> read(const std::string& filename);
    void write(const std::vector<Triangle>& soup, const std::string& filename);

    static bool approximatelyEqual(double a, double b, double epsilon);
};