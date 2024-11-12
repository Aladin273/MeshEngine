void View::decorateTriad(Triad& triad) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> arrowX = Mesh::createArrow(axisX, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowY = Mesh::createArrow(axisY, pointTR, pointTL, shaftTR, shaftTL, numSubs);
    std::unique_ptr<Mesh> arrowZ = Mesh::createArrow(axisZ, pointTR, pointTL, shaftTR, shaftTL, numSubs);

    std::unique_ptr<Mesh> torusX = Mesh::createTorus(axisX, minorTR, majorTR, numSubs);
    std::unique_ptr<Mesh> torusY = Mesh::createTorus(axisY, minorTR, majorTR, numSubs);
    std::unique_ptr<Mesh> torusZ = Mesh::createTorus(axisZ, minorTR, majorTR, numSubs);

    std::unique_ptr<Mesh> cubeX = Mesh::createCube(axisX * cubeTL, cubeTR);
    std::unique_ptr<Mesh> cubeY = Mesh::createCube(axisY * cubeTL, cubeTR);
    std::unique_ptr<Mesh> cubeZ = Mesh::createCube(axisZ * cubeTL, cubeTR);

    arrowX->setMaterial(MeshEngine::Settings::red);
    arrowY->setMaterial(MeshEngine::Settings::green);
    arrowZ->setMaterial(MeshEngine::Settings::blue);

    torusX->setMaterial(MeshEngine::Settings::red);
    torusY->setMaterial(MeshEngine::Settings::green);
    torusZ->setMaterial(MeshEngine::Settings::blue);

    cubeX->setMaterial(MeshEngine::Settings::red);
    cubeY->setMaterial(MeshEngine::Settings::green);
    cubeZ->setMaterial(MeshEngine::Settings::blue);

    triad.getChildren()[0]->attachMesh(std::move(arrowX));
    triad.getChildren()[1]->attachMesh(std::move(arrowY));
    triad.getChildren()[2]->attachMesh(std::move(arrowZ));

    triad.getChildren()[3]->attachMesh(std::move(torusX));
    triad.getChildren()[4]->attachMesh(std::move(torusY));
    triad.getChildren()[5]->attachMesh(std::move(torusZ));

    triad.getChildren()[6]->attachMesh(std::move(cubeX));
    triad.getChildren()[7]->attachMesh(std::move(cubeY));
    triad.getChildren()[8]->attachMesh(std::move(cubeZ));
}

void View::decorateArrow(Manipulator& manipulator, glm::vec3 dir) const
{
    using namespace Settings;

    std::unique_ptr<Mesh> mesh = Mesh::createArrow(dir, pointAR, pointAL, shaftAR, shaftAL, numSubs);

    mesh->setMaterial(MeshEngine::Settings::blue);
    manipulator.attachMesh(std::move(mesh));
}