#include "ColladaParser.h"

using namespace tinyxml2;

std::unique_ptr<Model> ColladaParser::loadModel(const std::string& filename) // Refact
{
    GeometryMap geometries;
    std::unique_ptr<Model> model = std::make_unique<Model>();

    std::stringstream stream;
    std::string element;

    XMLDocument doc;
    XMLElement* pNode, *pGeometry, *pMesh, *pVertices, *pInput, *pSource, *pPrimitive, *pData;

    doc.LoadFile(filename.c_str());

    if (doc.Error())
        return nullptr;

    pGeometry = doc.RootElement()->FirstChildElement("library_geometries")->FirstChildElement("geometry");

    while (pGeometry != nullptr)
    {
        Geometry geometry;
        pMesh = pGeometry->FirstChildElement("mesh");

        while (pMesh != nullptr)
        {
            pVertices = pMesh->FirstChildElement("vertices");
            pInput = pVertices->FirstChildElement("input");

            while (pInput != nullptr)
            {
                std::string source_name = pInput->Attribute("source");
                source_name.erase(source_name.begin());

                pSource = pMesh->FirstChildElement("source");

                while (pSource != nullptr)
                {
                    if (pSource->Attribute("id") == source_name)
                    {
                        if (pSource->FirstChildElement("float_array") != nullptr)
                            pData = pSource->FirstChildElement("float_array");
                        else if (pSource->FirstChildElement("int_array") != nullptr)
                            pData = pSource->FirstChildElement("int_array");
                        else
                            return nullptr;

                        stream.clear();
                        stream.str(pData->GetText());

                        std::vector<float> data;

                        while (stream >> element)
                        {
                            data.push_back(std::stof(element));
                        }

                        geometry.sources.emplace(pInput->Attribute("semantic"), data);
                    }

                    pSource = pSource->NextSiblingElement("source");
                }

                pInput = pInput->NextSiblingElement("input");
            }

            if (pMesh->FirstChildElement("triangles") != nullptr)
            {
                geometry.type = Primitives::Triangles;
                pPrimitive = pMesh->FirstChildElement("triangles");
            }
            else if (pMesh->FirstChildElement("polylist") != nullptr)
            {
                geometry.type = Primitives::Polylist;
                pPrimitive = pMesh->FirstChildElement("polylist");
                pData = pPrimitive->FirstChildElement("vcount");

                stream.clear();
                stream.str(pData->GetText());

                while (stream >> element)
                {
                    geometry.vcounts.push_back(std::stoi(element));
                }
            }
            else
                return nullptr;

            pData = pPrimitive->FirstChildElement("p");

            stream.clear();
            stream.str(pData->GetText());

            while (stream >> element)
            {
                geometry.indices.push_back(std::stoi(element));
            }

            // Remove info about normals, texcoords
            int64_t size, offset = 1;

            pData = pPrimitive->FirstChildElement("input");

            while (pData = pData->NextSiblingElement("input"))
                ++offset;

            size = geometry.indices.size() / offset;

            for (int64_t i = 0; i < size; ++i)
                geometry.indices[i] = geometry.indices[i * offset];

            geometry.indices.resize(size);

            pMesh = pMesh->NextSiblingElement("mesh");
        }

        geometries.emplace(pGeometry->Attribute("id"), geometry);

        pGeometry = pGeometry->NextSiblingElement("geometry");
    }

    pNode = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene")->FirstChildElement("node");

    while (pNode != nullptr)
    {
        std::unique_ptr<Node> node = loadNode(nullptr, pNode, geometries);

        if (node != nullptr)
            model->attachNode(std::move(node));

        pNode = pNode->NextSiblingElement("node");
    }

    model->setName(filename);

    return model;
}

std::unique_ptr<Node> ColladaParser::loadNode(Node* parent, XMLElement* pNode, const GeometryMap& geometries)
{
    std::unique_ptr<Node> node = std::make_unique<Node>();
    std::stringstream stream;
    std::string element;

    if (pNode->FirstChildElement("instance_geometry") == nullptr)
        return nullptr;

    if (pNode->Attribute("id"))
        node->setName(pNode->Attribute("id"));
    else if (pNode->Attribute("name"))
        node->setName(pNode->Attribute("name"));
    else
        return nullptr;

    for (auto& tranform : transforms)
    {
        XMLElement* pData = pNode->FirstChildElement(tranform.first.c_str());

        while (pData != nullptr)
        {
            std::vector<float> data;

            stream.clear();
            stream.str(pData->GetText());

            while (stream >> element)
            {
                data.push_back(std::stof(element));
            }

            node->applyRelativeTransform(tranform.second(data));
            pData = pNode->NextSiblingElement(tranform.first.c_str());
        }
    }

    Utils::HalfEdgeTable table;

    std::string geometryName = pNode->FirstChildElement("instance_geometry")->Attribute("url");
    geometryName.erase(geometryName.begin());

    auto geometry = geometries.find(geometryName);

    if (geometry != geometries.end())
    {
        auto source = (geometry->second.sources.find("POSITION"))->second;
        auto indices = geometry->second.indices;
        auto vcounts = geometry->second.vcounts;

        for (size_t index = 0; index < source.size(); index += 3)
            table.addVertex({ source[index], source[index + 1], source[index + 2] });

        if (geometry->second.type == Primitives::Polylist)
        {
            for (size_t index = 0, vcount = 0; index < indices.size() && vcount < vcounts.size(); ++vcount)
            {
                if (vcounts[vcount] == 3)
                {
                    table.addFace(Utils::VertexHandle{ indices[index] }, Utils::VertexHandle{ indices[index + 1] }, Utils::VertexHandle{ indices[index + 2] });
                    index += 3;
                }
                else if (vcounts[vcount] == 4)
                {
                    table.addFace(Utils::VertexHandle{ indices[index] }, Utils::VertexHandle{ indices[index + 1] }, Utils::VertexHandle{ indices[index + 2] }, Utils::VertexHandle{ indices[index + 3] });
                    index += 4;
                }
            }
        }
        else if (geometry->second.type == Primitives::Triangles)
        {
            for (size_t index = 0; index < indices.size(); index += 3)
            {
                table.addFace(Utils::VertexHandle{ indices[index] }, Utils::VertexHandle{ indices[index + 1] }, Utils::VertexHandle{ indices[index + 2] });
            }
        }

        table.connectTwins();
    }
    else
        return nullptr;

    node->attachMesh(std::make_unique<Mesh>(table));

    pNode = pNode->FirstChildElement("node");

    while (pNode != nullptr)
    {
        std::unique_ptr<Node> childNode = loadNode(node.get(), pNode, geometries);

        if (childNode != nullptr)
            node->attachNode(std::move(childNode));

        pNode = pNode->NextSiblingElement("node");
    }

    return node;
}

void ColladaParser::saveModel(const Model& model, const std::string& filename) // Refact
{
    if (model.getNodes().empty())
        return;

    XMLDocument doc;
    XMLElement* pCollada, * pScene, * pLib, * pNode;

    std::string url, file = filename;

    doc.LoadFile(file.c_str());

    if (doc.Error())
    {
        doc.Clear();
        pCollada = doc.NewElement("collada");
        pLib = pCollada->InsertNewChildElement("library_geometries");
        pScene = pCollada->InsertNewChildElement("library_visual_scenes")->InsertNewChildElement("visual_scene");
        pScene->SetAttribute("id", "Scene");
        doc.InsertFirstChild(pCollada);
    }
    else
    {
        file.pop_back(); file.pop_back(); file.pop_back(); file.pop_back();
        file = file + "_modified.dae";
    }

    pScene = doc.RootElement()->FirstChildElement("library_visual_scenes")->FirstChildElement("visual_scene");
    pNode = pScene->FirstChildElement("node");
    pLib = doc.RootElement()->FirstChildElement("library_geometries");
    pLib->DeleteChildren();

    // Delete useless geometry and nodes
    while (pNode != nullptr)
    {
        if (pNode->FirstChildElement("instance_geometry") == nullptr)
        {
            pNode = pNode->NextSiblingElement("node");
        }
        else
        {
            pScene->DeleteChild(pNode);
            pNode = pScene->FirstChildElement("node");
        }
    }

    // Save new geometry
    for (auto& node : model.getNodes())
    {
        saveNode(node.get(), pScene, pLib);
    }

    doc.SaveFile((file).c_str());
}

void ColladaParser::saveNode(Node* parent, tinyxml2::XMLElement* pParent, tinyxml2::XMLElement* pLib)
{
    std::stringstream stream;
    std::string element;

    XMLElement* pGeometry, * pMesh, * pVertices, * pInput, * pSource, * pPrimitive, * pData;

    pGeometry = pLib->InsertNewChildElement("geometry");
    pGeometry->SetAttribute("id", (parent->getName() + "-mesh").c_str());

    pMesh = pGeometry->InsertNewChildElement("mesh");

    pSource = pMesh->InsertNewChildElement("source");
    pSource->SetAttribute("id", (std::string(pGeometry->Attribute("id")) + "-positions").c_str());

    const Utils::HalfEdgeTable& table = parent->getMesh()->getHalfEdgeTable();

    pData = pSource->InsertNewChildElement("float_array");
    pData->SetAttribute("id", (std::string(pSource->Attribute("id")) + "-array").c_str());
    pData->SetAttribute("count", table.getVertices().size() * 3);

    stream.clear();
    stream.str("");

    for (auto& vertex : table.getVertices())
    {
        const glm::vec3& point = table.getPoint(table.handle(vertex));

        stream << point.x << " ";
        stream << point.y << " ";
        stream << point.z << " ";
    }

    pData->SetText(stream.str().c_str());

    pVertices = pMesh->InsertNewChildElement("vertices");
    pVertices->SetAttribute("id", (std::string(pGeometry->Attribute("id")) + "-vertices").c_str());

    pInput = pVertices->InsertNewChildElement("input");
    pInput->SetAttribute("semantic", "POSITION");
    pInput->SetAttribute("source", ("#" + std::string(pSource->Attribute("id"))).c_str());

    pPrimitive = pMesh->InsertNewChildElement("polylist");
    pPrimitive->SetAttribute("material", "Material1");
    pPrimitive->SetAttribute("count", table.getFaces().size());

    pInput = pPrimitive->InsertNewChildElement("input");
    pInput->SetAttribute("semantic", "VERTEX");
    pInput->SetAttribute("offset", 0);
    pInput->SetAttribute("source", ("#" + std::string(pVertices->Attribute("id"))).c_str());

    element.clear();
    stream.clear();
    stream.str("");

    for (auto& face : table.getFaces())
    {
        Utils::HalfEdgeHandle start_heh = face.heh;
        Utils::HalfEdgeHandle next_heh = face.heh;

        int64_t size = 0;

        do
        {
            ++size;
            stream << table.destVertex(next_heh).index << " ";
            next_heh = table.next(next_heh);
        } while (next_heh != start_heh);

        element += std::to_string(size) + " ";
    }

    pPrimitive->InsertNewChildElement("vcount")->SetText(element.c_str());
    pPrimitive->InsertNewChildElement("p")->SetText(stream.str().c_str());

    pParent = pParent->InsertNewChildElement("node");
    pParent->SetAttribute("id", parent->getName().c_str());
    pParent->SetAttribute("type", "NODE");
    pParent->InsertNewChildElement("instance_geometry")->SetAttribute("url", ("#" + std::string(pGeometry->Attribute("id"))).c_str());

    stream.clear();
    stream.str("");

    for (int i = 0; i < parent->getRelativeTransform().length(); ++i)
        for (int j = 0; j < parent->getRelativeTransform()[i].length(); ++j)
            stream << parent->getRelativeTransform()[j][i] << " ";

    pParent->InsertNewChildElement("matrix")->SetText(stream.str().c_str());

    for (auto& child : parent->getChildren())
    {
        saveNode(child.get(), pParent, pLib);
    }
}