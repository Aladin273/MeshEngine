#include "STLParser.h"

STLParser::TriangleSoup STLParser::read(const std::string& filename)
{
    TriangleSoup soup;
    Triangle triangle;

    std::string read;
    std::ifstream fin;
    fin.open(filename);

    if (fin.is_open()) // May be remake to regex
    {
        while (read != "solid" && !fin.eof() && !fin.fail()) // Search file.begin
            fin >> read;

        while (read == "solid" || !fin.eof()) // Search triangle
        {
            fin.clear();

            while (read != "facet" && !fin.eof() && !fin.fail())
                fin >> read;

            while (read != "normal" && !fin.eof() && !fin.fail())
                fin >> read;

            fin >> triangle.normal.x;
            fin >> triangle.normal.y;
            fin >> triangle.normal.z;

            while (read != "outer" && !fin.eof() && !fin.fail())
                fin >> read;

            while (read != "loop" && !fin.eof() && !fin.fail())
                fin >> read;

            while (read != "vertex" && !fin.eof() && !fin.fail())
                fin >> read;

            fin >> triangle.A.x;
            fin >> triangle.A.y;
            fin >> triangle.A.z;
            fin >> read;

            fin >> triangle.B.x;
            fin >> triangle.B.y;
            fin >> triangle.B.z;
            fin >> read;

            fin >> triangle.C.x;
            fin >> triangle.C.y;
            fin >> triangle.C.z;
            fin >> read;

            while (read != "endloop" && !fin.eof() && !fin.fail())
                fin >> read;

            while (read != "endfacet" && !fin.eof() && !fin.fail())
                fin >> read;

            if (!fin.eof() && !fin.fail())
            {
                soup.push_back(triangle);
            }
        }
    }
    fin.close();

    return soup;
}

void STLParser::write(const TriangleSoup& soup, const std::string& filename)
{
    if (!soup.empty())
    {
        std::ofstream fout;
        fout.open(filename);

        if (fout.is_open())
        {
            fout << "solid object" << std::endl << std::endl;;

            for (auto& triangle : soup)
            {
                fout << "facet normal " << triangle.normal.x << " " << triangle.normal.y << " " << triangle.normal.z << std::endl;
                fout << "outer loop" << std::endl;
                fout << "vertex " << triangle.A.x << " " << triangle.A.y << " " << triangle.A.z << std::endl;
                fout << "vertex " << triangle.B.x << " " << triangle.B.y << " " << triangle.B.z << std::endl;
                fout << "vertex " << triangle.C.x << " " << triangle.C.y << " " << triangle.C.z << std::endl;
                fout << "endloop" << std::endl;
                fout << "endfacet" << std::endl << std::endl;
            }

            fout << "endsolid object" << std::endl;
            fout.close();
        }
    }
}

std::unique_ptr<Node> STLParser::loadNode(const std::string& filename)
{
    TriangleSoup soup = read(filename);

    std::map<Vec, heds::VertexHandle> vertices;
    std::map<Vec, heds::VertexHandle>::iterator it0;
    std::map<Vec, heds::VertexHandle>::iterator it1;
    std::map<Vec, heds::VertexHandle>::iterator it2;

    heds::HalfEdgeTable halfEdgeTable;
    heds::VertexHandle vh0;
    heds::VertexHandle vh1;
    heds::VertexHandle vh2;

    for (auto& triangle : soup)
    {
        it0 = vertices.find(triangle.A);
        it1 = vertices.find(triangle.B);
        it2 = vertices.find(triangle.C);

        if (it0 == vertices.end())
        {
            vh0 = halfEdgeTable.addVertex({ triangle.A.x, triangle.A.y, triangle.A.z });
            vertices.emplace(triangle.A, vh0);
        }
        else
            vh0 = it0->second;

        if (it1 == vertices.end())
        {
            vh1 = halfEdgeTable.addVertex({ triangle.B.x, triangle.B.y, triangle.B.z });
            vertices.emplace(triangle.B, vh1);
        }
        else
            vh1 = it1->second;

        if (it2 == vertices.end())
        {
            vh2 = halfEdgeTable.addVertex({ triangle.C.x, triangle.C.y, triangle.C.z });
            vertices.emplace(triangle.C, vh2);
        }
        else
            vh2 = it2->second;

        halfEdgeTable.addFace(vh0, vh1, vh2);
    }

    halfEdgeTable.connectTwins();

    std::unique_ptr<Node> node = std::make_unique<Node>();
    node->attachMesh(std::make_unique<Mesh>(halfEdgeTable));
    node->setName(filename);

    return node;
}

std::unique_ptr<Model> STLParser::loadModel(const std::string& filename)
{
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->attachNode(loadNode(filename));
    model->setName(filename);

    return model;
}

void STLParser::saveNode(TriangleSoup& soup, Node* node)
{
    const heds::HalfEdgeTable& table = node->getMesh()->getHalfEdgeTable();

    for (auto& face : table.getFaces())
    {
        heds::HalfEdgeHandle heh0 = face.heh;
        heds::HalfEdgeHandle heh1 = table.next(heh0);
        heds::HalfEdgeHandle heh2 = table.next(heh1);
        heds::HalfEdgeHandle heh3 = table.next(heh2);

        const glm::vec3& a = table.getEndPoint(heh0);
        const glm::vec3& b = table.getEndPoint(heh1);
        const glm::vec3& c = table.getEndPoint(heh2);

        glm::vec3 normal = glm::normalize(glm::cross(b - a, c - b));

        soup.push_back(Triangle{ {a.x, a.y, a.z}, {b.x, b.y, b.z}, {c.x, c.y, c.z}, {normal.x, normal.y, normal.z} });

        if (heh3 != heh0) // if 4 vertices
        {
            const glm::vec3& d = table.getEndPoint(heh3);
            soup.push_back(Triangle{ {c.x, c.y, c.z}, {d.x, d.y, d.z}, {a.x, a.y, a.z}, {normal.x, normal.y, normal.z} });
        }
    }

    for (auto& child : node->getChildren())
        saveNode(soup, child.get());
}

void STLParser::saveModel(const Model& model, const std::string& filename)
{
    TriangleSoup soup;

    for (auto& node : model.getNodes())
    {
        saveNode(soup, node.get());
    }

    write(soup, filename);
}

bool STLParser::approximatelyEqual(double a, double b, double epsilon)
{
    return fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * epsilon);
}

bool STLParser::Vec::operator<(const Vec& b) const
{
    return x < b.x || approximatelyEqual(x, b.x, epsilon) && (y < b.y || approximatelyEqual(y, b.y, epsilon) && z < b.z);
}

bool STLParser::Vec::operator>(const Vec& b) const
{
    return x > b.x || approximatelyEqual(x, b.x, epsilon) && (y > b.y || approximatelyEqual(y, b.y, epsilon) && z > b.z);
}

bool STLParser::Vec::operator==(const Vec& b) const
{
    return approximatelyEqual(x, b.x, epsilon) && approximatelyEqual(y, b.y, epsilon) && approximatelyEqual(z, b.z, epsilon);
}

bool STLParser::Vec::operator!=(const Vec& b) const
{
    return !(approximatelyEqual(x, b.x, epsilon) && approximatelyEqual(y, b.y, epsilon) && approximatelyEqual(z, b.z, epsilon));
}
