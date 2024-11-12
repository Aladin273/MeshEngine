#include "PlaneNode.h"

#include "MeshEngine/Misc/Settings.h"

PlaneNode::PlaneNode(glm::vec3 worldUp, float width, float height, uint32_t numSubs)
{
    std::unique_ptr<Mesh> mesh = Mesh::createPlane(worldUp, width, height, numSubs);

    mesh->renderTriangles = false;
    mesh->renderLines = true;
    mesh->colorLines = MeshEngine::Settings::colorGray;

    attachMesh(std::move(mesh));
}

