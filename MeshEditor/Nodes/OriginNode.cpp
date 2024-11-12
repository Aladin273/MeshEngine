#include "OriginNode.h"

#include "MeshEngine/Misc/Settings.h"

OriginNode::OriginNode()
{
    using namespace MeshEngine::Settings;

    // Arrow
    const float pointAR = 0.1f;
    const float shaftAR = 0.05f;
    const float pointAL = 0.2f;
    const float shaftAL = 0.8f;
    const float sizeA = 0.15f;

    // Triad
    const float pointTR = 0.06f;
    const float shaftTR = 0.02f;
    const float pointTL = 0.1f;
    const float shaftTL = 0.8f;
    const float minorTR = 0.02f;
    const float majorTR = 0.98f;
    const float cubeTR = 0.1f;
    const float cubeTL = 0.6f;
    const float sizeT = 0.15f;
    const uint32_t numSubs = 32;

    std::unique_ptr<Mesh> arrowX = Mesh::createArrow(axisX, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowY = Mesh::createArrow(axisY, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowZ = Mesh::createArrow(axisZ, pointTR, pointTL, shaftTR, shaftTL, numSubs);

    arrowX->setMaterial(materialRed);
    arrowY->setMaterial(materialGreen);
    arrowZ->setMaterial(materialBlue);

    attachNode(std::make_unique<MeshNode>());
    attachNode(std::make_unique<MeshNode>());
    attachNode(std::make_unique<MeshNode>());

    dynamic_cast<MeshNode*>(getChildren()[0].get())->attachMesh(std::move(arrowX));
    dynamic_cast<MeshNode*>(getChildren()[1].get())->attachMesh(std::move(arrowY));
    dynamic_cast<MeshNode*>(getChildren()[2].get())->attachMesh(std::move(arrowZ));
}

